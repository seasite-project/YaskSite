#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "parse.h"
#include <likwid.h>


#define DISABLE_PREFETCHER(nthreads) \
    printf("Disabling prefetcher\n");\
    for(int i=0; i<nthreads; ++i)\
    {\
        cpuFeatures_disable(i, FEAT_HW_PREFETCHER, 0); \
        cpuFeatures_disable(i, FEAT_CL_PREFETCHER, 0); \
        cpuFeatures_disable(i, FEAT_DCU_PREFETCHER, 0); \
        cpuFeatures_disable(i, FEAT_IP_PREFETCHER, 0);\
    }\

#define ENABLE_PREFETCHER(nthreads) \
    printf("Enabling prefetcher\n");\
    for(int i=0; i<nthreads; ++i)\
    {\
        cpuFeatures_enable(i, FEAT_HW_PREFETCHER, 0); \
        cpuFeatures_enable(i, FEAT_CL_PREFETCHER, 0); \
        cpuFeatures_enable(i, FEAT_DCU_PREFETCHER, 0); \
        cpuFeatures_enable(i, FEAT_IP_PREFETCHER, 0);\
    }\


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
    printf("dt=%f\n", dt);\
    printf("dx X dy X dz = %f X %f X %f\n",dx,dy,dz);\
    printf("bx X by X bz = %f X %f X %f\n",stencil->bx,stencil->by,stencil->bz);\
    printf("L3_pred = %f, L3_measure = %f\n",ECM[2],ECM_validate[2]);\
    printf("L2_pred = %f, L2_measure = %f\n",ECM[1],ECM_validate[1]);\
    printf("L1_pred = %f, L1_measure = %f\n",ECM[0],ECM_validate[0]);\
    printf("MLUPS_pred = %f, MLUPS_measure = %f\n", ECM_mlups, mlups/time);\
}\

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    cpuFeatures_init();


    //generate code and specify in kernel i/p if needed
    CODIFY( "heat2d_update", "heat2d",
            "PARAM dt;\n"
            "OUT = INP + dt*STENCIL(INP);\n"
          );

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

    for(int dim=start_dim; dim<end_dim; dim+=inc_dim)
    {
        //taking care size always fits only in memory
        //(here 200 times L3 is the total size)
        cache_info L3 = CACHE("L2"); //L3_cache macro defined by yaskSite library
        int L3_size = L3.bytes;
        int dim_x=1, dim_y=1, dim_z=1;

        if(dimension == 3)
        {

            dim_x = 32;//static_cast<int>((200.0*L3_size)/((double)(2.0*8.0*dim*dim)));
            dim_y = 500;
           // dim_z = 1024*4 - cannot be blocked in L2 & L1; however small you make the block size due to thrashing, all mapped to same set
            dim_z = 500*10; //1032*4 - min of 5 words extra for bz>16; here till bz=496*8 5 words


            //nicely spread first break at bz=240*8; then 256*8,271*8,....
           /* dim_x = 64;
            dim_y = 512;
            dim_z = 1087*2;*/
        }
        else if(dimension==2)
        {
            dim_y = static_cast<int>((200.0*L3_size)/((double)(2.0*8.0*dim)));
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

        DISABLE_PREFETCHER(threads);

        //stencil_1->spatialTuner("L3", "L2");
        // stencil_1->setBlock(1500,512,32);
        stencil_1->setBlock(500,500,32);
        //stencil_1->setSubBlock(24,24,192);
        // stencil_1->setBlock(192,48,dim_x);
        PERF_RUN(stencil_1, files[1]);

        ENABLE_PREFETCHER(threads);

        //free stencil_1
        delete stencil_1;
    }


    for(int i=0; i<3; ++i)
    {
        free(files[i]);
    }
}

