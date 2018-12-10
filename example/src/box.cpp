#include <yaskSite.h>
#include "timing.h"

#define PERF_RUN(stencil)\
{\
    INIT_TIME(stencil_);\
    START_TIME(stencil_);\
    stencil.run();\
    STOP_TIME(stencil_);\
    double dt=stencil.dt;\
    double dx=stencil.dx;\
    double dy=stencil.dy;\
    double dz=stencil.dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    printf("Perf = %f MLUP/s, Time = %fs\n", mlups/time, time);\
}\

void main(int argc, char** argv)
{
    MPI_Manager mpiMan(&argc, &argv);

    //2D example
    yaskSite stencil_1(&mpiMan, "box2d", 2, 1, 1, 1);
    stencil_1.setThread(10,1);
    stencil_1.setDim(60000, 2000, 1, 50);
    stencil_1.run();//init goes here
    PERF_RUN(stencil_1);

    //block in L2
    stencil_1.spatialTuner("L2");
    PERF_RUN(stencil_1);

    //after temporal blocking one needs to call for spatial
    //stencil_1.temporalTuner("L3");
    //stencil_1.spatialTuner("L3");
    //alternative
    stencil_1.blockTuner("L3","L2");
    PERF_RUN(stencil_1);



    //3D example
    yaskSite stencil_2(&mpiMan, "box3d", 3, 1,1,1);
    stencil_2.setThread(10,1);
    stencil_2.setDim(620, 620, 852, 10);


    stencil_2.init();//init goes here
    PERF_RUN(stencil_2);

    stencil_2.spatialTuner("L3", "L2", 0.4, 0.4);
    PERF_RUN(stencil_2);

    //after temporal blocking one needs to call for spatial
    //stencil_1.temporalTuner("L3");
    //stencil_1.spatialTuner("L3", "L2");
    //alternative
    stencil_2.blockTuner("L3","L3","L2", 0.4, 0.4, 0.4);
    PERF_RUN(stencil_2);

}
