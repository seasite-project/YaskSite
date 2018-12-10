#include <yaskSite.h>
#include "timing.h"
//#include  <likwid.h>

#ifdef LIKWID_PERFMON
#define PERF_RUN(stencil,...)\
{\
    INIT_TIME(stencil_);\
    START_TIME(stencil_);\
    _Pragma("omp parallel")\
    {\
        LIKWID_MARKER_START(__VA_ARGS__);\
    }\
    stencil->run();\
    _Pragma("omp parallel")\
    {\
        LIKWID_MARKER_STOP(__VA_ARGS__);\
    }\
    STOP_TIME(stencil_);\
    double dt=stencil->dt;\
    double dx=stencil->dx;\
    double dy=stencil->dy;\
    double dz=stencil->dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    printf("Perf = %f MLUP/s, Time = %fs\n", mlups/time, time);\
}\

#else

#define PERF_RUN(stencil,...)\
{\
    INIT_TIME(stencil_);\
    START_TIME(stencil_);\
    stencil->run();\
    STOP_TIME(stencil_);\
    double dt=stencil->dt;\
    double dx=stencil->dx;\
    double dy=stencil->dy;\
    double dz=stencil->dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    printf("Perf = %f MLUP/s, Time = %fs\n", mlups/time, time);\
}\

#endif

void main(int argc, char** argv)
{
    MPI_Manager mpiMan(&argc, &argv);

#ifdef LIKWID_PERFMON
    LIKWID_MARKER_INIT;

#pragma omp parallel
    {
        LIKWID_MARKER_THREADINIT;
    }
#endif

    double* dt;
    //Generating code examples
    //1. heat2d_update
    CODIFY( "heat2d_update", "heat2d",
            "PARAM dt;\n"
            "OUT = INP + dt*STENCIL(INP);\n"
          );

    /*    yaskSite* stencil_0 = new yaskSite(&mpiMan, "heat2d_update",2,1,1,1);
          stencil_0->setThread(10,1);
          stencil_0->setDim(60000, 2000, 1, 50);
          stencil_0->init();//init goes here
          dt = stencil_0->getParam("dt"); //set the defined parameters
          (*dt) = 0.025;
          double* h_inv_sq = stencil_0->getParam("h_inv_sq"); //set the defined parameters
          (*h_inv_sq) = 1000;


          PERF_RUN(stencil_0);

          stencil_0->spatialTuner("L2");
          PERF_RUN(stencil_0);

          stencil_0->blockTuner("L3", "L2");
          PERF_RUN(stencil_0);

          delete stencil_0;
          */

    //2. RK(f23) method
    CODIFY( "heat2d_rk", "heat2d",
            "GRID k1, k2;\n"
            "GRID_POINT k3;\n"
            "PARAM dt;\n"
            "k1  = STENCIL(INP);\n"
            "k2  = k1 + dt*1.0*STENCIL(k1);\n"
            "k3  = k1 + (0.25)*(k2-k1) + dt*0.25*STENCIL(k2);\n"
            "OUT = INP + dt*(0.5*k1 + 0.5*k2 + 0*k3);\n"
          );


    //OR
    /*
       CODIFY("test", "heat2d",
       "GRID k1, k2;\n"
       "GRID_POINT k3;\n"
       "PARAM dt;\n"
       "k1 = STENCIL(INP);\n"
       "k2 = k1 + dt*%f*STENCIL(k1);\n"
       "k3 = k1 + (%f/%f)*(k2-k1) + dt*%f*STENCIL(k2);\n"
       "OUT = INP + dt*(%f*k1 + %f*k2 + %f*k3);\n", 1.0, 0.25, 1.0, 0.25, 0.5, 0.5, 0
       );
       */
/*
    yaskSite* stencil_1 = new yaskSite(&mpiMan, "heat2d_rk",2,1,1,1);
    stencil_1->setThread(1,1);
    stencil_1->setDim(80000, 2000, 1, 50);
    stencil_1->init();//init goes here
    dt = stencil_1->getParam("dt"); //set the defined parameters
    (*dt) = 0.025;*/
/*
    PERF_RUN(stencil_1);

    stencil_1->spatialTuner("L2", "L2", 0.4);
    PERF_RUN(stencil_1);
*/
    /*
    stencil_1->blockTuner("L3", "L2", "L2");
    PERF_RUN(stencil_1, "RKF23-temporal");

    delete stencil_1;
*/
    yaskSite* stencil_0 = new yaskSite(&mpiMan, "heat2d",2,1,1,1);
    stencil_0->setThread(1,1);
    stencil_0->setDim(80000, 2000, 1, 50);
    stencil_0->init();//init goes here
/*    dt = stencil_0->getParam("dt"); //set the defined parameters
    (*dt) = 0.025;
*/
    stencil_0->blockTuner("L3", "L2", "L2");
    PERF_RUN(stencil_0, "heat2d");

    delete stencil_0;


    //2. EMBRK(f23) method
    CODIFY( "heat2d_embrk", "heat2d",
            "GRID k1, k2, ERR;\n"
            "GRID_POINT k3;\n"
            "PARAM dt;\n"
            "k1  = STENCIL(INP);\n"
            "k2  = k1 + dt*1.0*STENCIL(k1);\n"
            "k3  = k1 + (0.25)*(k2-k1) + dt*0.25*STENCIL(k2);\n"
            "OUT = INP + dt*(0.5*k1 + 0.5*k2 + 0*k3);\n"
            "ERR = (1/6.0-0.5)*k1 + (1/6.0-0.5)*k2 + (4/6.0)*k3;\n"
        );

    yaskSite* stencil_2 = new yaskSite(&mpiMan, "heat2d_embrk",2,1,1,1);
    stencil_2->setThread(1,1);
    stencil_2->setDim(80000, 2000, 1, 50);
    stencil_2->init();//init goes here
    dt = stencil_2->getParam("dt"); //set the defined parameters
    (*dt) = 0.025;

  /*  PERF_RUN(stencil_2,"plain");

    stencil_2->spatialTuner("L3", "L2");
    PERF_RUN(stencil_2, "spatial");
*/
    stencil_2->blockTuner("L3", "L3", "L2");
    PERF_RUN(stencil_2, "temporal");

    delete stencil_2;

#ifdef LIKWID_PERFMON
    LIKWID_MARKER_CLOSE;
#endif

}
