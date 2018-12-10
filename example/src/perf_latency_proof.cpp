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
    stencil->calcECM(true);\
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
    contribution ecm_contrib = stencil->getECMContributions();\
    fprintf(writeFile, "%3d, %5d, %5d, %5d, %8.5f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.4f, %8.4f, %8.4f, %8.4f\n", stencil->dt, stencil->dx, stencil->dy, stencil->dz, time,mlups/time, ECM_validate[0], ECM_validate[1], ECM_validate[2], ECM_mlups, ECM[0], ECM[1], ECM[2], ecm_contrib.latency*100, ecm_contrib.boundary*100, ecm_contrib.prefetch*100, ecm_contrib.associativity*100);\
   if(file!=NULL) fclose(writeFile);\
}\

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    MPI_Manager mpiMan(&argc, &argv);

    parser optParse;
    optParse.parse_arg(argc, argv);

    bool prefetch=optParse.prefetch;
    std::vector<char*> kernelFullDesc = splitChar(optParse.kernel);
    char* kernel = kernelFullDesc[0];
    int dimension = atoi(kernelFullDesc[1]);

    std::vector<int> range = getRange(optParse.innerDim);
    int start_dim = range[0];
    int end_dim = range[2];
    int inc_dim = range[1];

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

    for(int dim=start_dim; dim<end_dim; dim+=inc_dim)
    {
        //taking care size always fits only in memory
        //(here 200 times L3 is the total size)
        cache_info L3 = CACHE("L3"); //L3_cache macro defined by yaskSite library
        int L3_size = L3.bytes;
        int dim_x=1, dim_y=1, dim_z=1;

        dim = threads*240;

        if(dimension == 3)
        {
            dim_x = 30;//static_cast<int>((70*L3_size)/((double)(2.0*8.0*dim*dim)));
            dim_y = 4800;
            dim_z = dim;
        }
        else if(dimension==2)
        {
            dim_y = static_cast<int>((70.0*L3_size)/((double)(2.0*8.0*dim)));
            dim_z = dim;
        }
        else
        {
            printf("Unknown stencil dimension\n");
        }

        yaskSite* stencil_1 = new yaskSite(&mpiMan, kernel, dimension, radius, f_z, f_y, f_x, prefetch);
        stencil_1->setDim(dim_z, dim_y, dim_x, dt);
        stencil_1->setThread(threads,1);
        stencil_1->init();//init goes here, warm-up

        /*stencil_1->spatialTuner("L3", "L2");*/

        /*            if(threads == 9)
                      stencil_1->setBlock(240,dim/3,dim_x);
                      if(threads == 8)
                      stencil_1->setBlock(240,dim/2,dim_x);
                      else*/

        stencil_1->setBlock(240,dim_y,dim_x);
        PERF_RUN(stencil_1, files[1]);

        //free stencil_1
        delete stencil_1;
    }

    for(int i=0; i<3; ++i)
    {
        free(files[i]);
    }
}

