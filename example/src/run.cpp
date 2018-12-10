#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "parse.h"

#define PRINT_BANNER(file)\
{\
    FILE* writeFile;\
    if(file)\
    {\
        writeFile = (file!=NULL) ?fopen(file,"a"):stdout;\
        fprintf(writeFile, "%3s\t %5s\t %5s\t %5s\t %9s\t %9s\t %6s %6s %6s %9s %6s %6s %6s\n", "dt", "dx", "dy", "dz",  "s", "mlups", "MEM", "L3", "L2", "ECM_mlups", "MEM", "L3", "L2");\
        if(file!=NULL) fclose(writeFile);\
    }\
}

#define PERF_RUN(stencil, file)\
{\
    FILE* writeFile;\
    writeFile = (file!=NULL) ?fopen(file,"a"):stdout;\
    INIT_TIME(stencil_);\
    bool validate = true;\
    stencil->calcECM(validate);\
    stencil->printECM();\
    START_TIME(stencil_);\
    stencil->run();\
    std::vector<double> ECM = stencil->getECM(true);\
    std::vector<double> ECM_validate = stencil->getECM_validate(true); \
    double ECM_mlups = stencil->getPerfECM();\
    STOP_TIME(stencil_);\
    double dt=stencil->dt;\
    double dx=stencil->dx;\
    double dy=stencil->dy;\
    double dz=stencil->dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    fprintf(writeFile, "%3d\t %5d\t %5d\t %5d\t %9.6f\t %9.3f\t %6.2f %6.2f %6.2f %9.3f\t %6.2f %6.2f %6.2f\n", stencil->dt, stencil->dx, stencil->dy, stencil->dz, time,mlups/time, ECM_validate[0], ECM_validate[1], ECM_validate[2], ECM_mlups, ECM[0], ECM[1], ECM[2]);\
    if(file!=NULL) fclose(writeFile);\
}\

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    //generate code and specify in kernel i/p if needed
    CODIFY( "heat2d_update", "heat2d",
            "PARAM dt;\n"
            "DATA<1> = DATA<0> + dt*STENCIL(DATA<0>);\n"
          );

    CODIFY( "heat2d_embrk", "heat2d",
            "GRID k1, k2, ERR;\n"
            "GRID_POINT k3;\n"
            "PARAM dt;\n"
            "k1<0>  = STENCIL(DATA<0>);\n"
            "k2<0>  = k1<0> + dt*1.0*STENCIL(k1<0>);\n"
            "k3  = k1<0> + (0.25)*(k2<0>-k1<0>) + dt*0.25*STENCIL(k2<0>);\n"
            "DATA<1> = DATA<0> + dt*(0.5*k1<0> + 0.5*k2<0> + 0*k3);\n"
            "ERR<0> = (1/6.0-0.5)*k1<0> + (1/6.0-0.5)*k2<0> + (4/6.0)*k3;\n"
          );


    MPI_Manager mpiMan(&argc, &argv);

    parser optParse;
    optParse.parse_arg(argc, argv);

    bool prefetch=optParse.prefetch;
    std::vector<char*> kernelFullDesc = splitChar(optParse.kernel);
    char* kernel = kernelFullDesc[0];
    int dimension = atoi(kernelFullDesc[1]);

    std::vector<int> range = getRange(optParse.innerDim);
    int dim_x = range[0];
    int dim_y = range[2];
    int dim_z = range[1];

    std::vector<int> fold = getRange(optParse.fold);
    int f_x = fold[0];
    int f_y = fold[1];
    int f_z = fold[2];

    int threads = optParse.cores;
    int dt = optParse.iter;

    int radius = optParse.radius;

    char* opt_range = optParse.opt;
    std::vector<char*> opt = splitChar(opt_range);
    std::vector<bool> opt_bool(3,false);

    if(findChar(opt,"plain")>=0)
    {
        opt_bool[0] = true;
    }
    if(findChar(opt,"spatial")>=0)
    {
        opt_bool[1] = true;
    }
    if(findChar(opt,"temporal")>=0)
    {
        opt_bool[2] = true;
    }

    char* outDir = optParse.outDir;
    std::vector<char*> files(3, NULL);
    if(outDir != NULL)
    {
        //for default path
        char* file_path;
        STRINGIFY(file_path, "%s/%s_%d_%s_%s_%d_%d_%d", outDir, kernel, radius, (prefetch)?"prefetch":"no_prefetch", optParse.path, f_x, f_y, f_z);

        //create directories if not present
        char *mkdirCmd;
        STRINGIFY(mkdirCmd, "mkdir -p %s", file_path);
        system(mkdirCmd);
        free(mkdirCmd);

        if(opt_bool[0])
            STRINGIFY(files[0],"%s/plain.txt",file_path);
        if(opt_bool[1])
            STRINGIFY(files[1],"%s/spatial.txt",file_path);
        if(opt_bool[2])
            STRINGIFY(files[2],"%s/temporal.txt",file_path);

        free(file_path);
    }

    for(int i=0; i<3; ++i)
    {
        if(opt_bool[i])
        {
            PRINT_BANNER(files[i]);
        }
    }

    yaskSite* stencil = new yaskSite(&mpiMan, kernel, dimension, radius, f_z, f_y, f_x, prefetch);
    stencil->setDim(dim_z, dim_y, dim_x, dt);
    stencil->setThread(threads,1);
    stencil->init();//init goes here, warm-up

    double* h_inv = stencil->getParam("h_inv");
    (*h_inv) = (double)(dim_z-1.0)*(double)(dim_y-1.0);
    double* dt_param = stencil->getParam("dt");
    (*dt_param) = 1e-5;//initial_stepsize(); //can change it during run

    for (int i = 0; i < dim_y; i++)
    {
        for (int j = 0; j < dim_z; j++)
        {
            double val = 0.5 + (double)j / (double)(dim_z - 1);
            stencil->setInputAt(val, "data", 0, i, j);
        }
    }

    stencil->write2dFile("iv.dat", "data", "y:z", 1);

    stencil->run();

    stencil->write2dFile("soln.dat", "data", "y:z", 1);

    delete stencil;

    for(int i=0; i<3; ++i)
    {
        free(files[i]);
    }
}

