#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "parse.h"

#define COUPLE_GRID

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    ////////////////Code for IRK method s=4/////////////////////////
    double A[4][4] = {
        {1,2,3,4},
        {1,2,3,4},
        {1,2,3,4},
        {1,2,3,4}   };

    double b[4] = {1,2,3,4};

    char *codeStr, *codeStr_cpy;

    STRINGIFY(codeStr,  "GRID_ARRAY F[24];\n"
            "GRID_POINT tmp;\n"
            "GRID_POINT_ARRAY tmp[4];\n"
            "PARAM h;\n");
    STRINGIFY_ADD(codeStr,"tmp = STENCIL(DATA<0>);\n");
    for(int i=0; i<4; ++i)
    {
        for(int l=0; l<4; ++l)
        {
            STRINGIFY_ADD(codeStr, "tmp[%d] += %f*tmp;\n", l, A[l][i]);
        }
    }

    for(int l=0; l<4; ++l)
    {
        STRINGIFY_ADD(codeStr, "F[%d]<0> = tmp[%d]*h + DATA<0>;\n", l, l);
    }

    for(int k=1; k<6; ++k)
    {
        for(int i=0; i<4; ++i)
        {
            STRINGIFY_ADD(codeStr,"tmp = STENCIL(F[%d]<0>);\n", i+4*(k-1));
            for(int l=0; l<4; ++l)
            {
                STRINGIFY_ADD(codeStr, "tmp[%d] += %f*tmp;\n", l, A[l][i]);
            }
        }
        for(int l=0; l<4; ++l)
        {
            STRINGIFY_ADD(codeStr, "F[%d]<0> = tmp[%d]*h + DATA<0>;\n", l+4*k, l);
        }
    }

    STRINGIFY_ADD(codeStr,"tmp = %f*STENCIL(F[%d]<0>);\n", b[0], 0+4*5);
    for(int l=1; l<4; ++l)
    {
        STRINGIFY_ADD(codeStr,"tmp = tmp +  %f*STENCIL(F[%d]<0>);\n", b[l], l+4*5);
    }

    STRINGIFY_ADD(codeStr, "DATA<1> = DATA<0> + h*tmp;\n");

    codeGen code("heat2d_irk", "heat2d", codeStr);

    free(codeStr);

    MPI_Manager mpiMan(&argc, &argv);

    parser optParse;
    optParse.parse_arg(argc, argv);

    bool prefetch=optParse.prefetch;
    //std::vector<char*> kernelFullDesc = splitChar(optParse.kernel);
    //char* kernel = kernelFullDesc[0];
    //int dimension = atoi(kernelFullDesc[1]);
    int dimension=2;

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
        STRINGIFY(file_path, "%s/%s_%d_%s_%s_%d_%d_%d", outDir, "heat2d_irk", radius, (prefetch)?"prefetch":"no_prefetch", optParse.path, f_x, f_y, f_z);

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

    for(int dim=start_dim; dim<end_dim; dim+=inc_dim)
    {
        //taking care size always fits only in memory
        //(here 200 times L3 is the total size)
        cache_info L3 = CACHE("L2"); //L3_cache macro defined by yaskSite library
        int L3_size = L3.bytes;
        int dim_x=1, dim_y=1, dim_z=1;

        if(dimension == 3)
        {
            dim_x = 512;//static_cast<int>((200.0*L3_size)/((double)(2.0*8.0*dim*dim)));
            dim_y = dim;
            dim_z = dim;
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

        yaskSite* stencil_1 = new yaskSite(&mpiMan, "heat2d_irk", dimension, radius, f_z, f_y, f_x, prefetch);
        stencil_1->setDim(dim_z, dim_y, dim_x, 1);
        stencil_1->setThread(threads,1);
        stencil_1->init();//init goes here, warm-up

        //plain run
        if(opt_bool[0])
        {
            stencil_1->calcECM(false);

            double ECM_1 = stencil_1->getPerfECM();
            double pred_time = 1/ECM_1;
            printf("plain prediction: mlups = %f\n", 1/pred_time);

            INIT_TIME(stencil_);
            START_TIME(stencil_);
            stencil_1->run();
            STOP_TIME(stencil_);
            double time = GET_TIME(stencil_);
            double mlups = dim_x*dim_y*dim_z/time;
            printf("plain: mlups = %f\n", mlups*1e-6);
        }

        //spatial blocked
        if(opt_bool[1])
        {
            stencil_1->spatialTuner("L3", "L2");

            double ECM_1 = stencil_1->getPerfECM();
            double pred_time = 1/ECM_1;
            printf("spatial prediction: mlups = %f\n", 1/pred_time);

            INIT_TIME(stencil_);
            START_TIME(stencil_);
            stencil_1->run();
            STOP_TIME(stencil_);
            double time = GET_TIME(stencil_);
            double mlups = dim_x*dim_y*dim_z/time;
            printf("spatial: mlups = %f\n", mlups*1e-6);
        }

        //temporal & spatial blocked
        if(opt_bool[2])
        {
            stencil_1->blockTuner("L3","L3","L2", 0.5);

            double ECM_1 = stencil_1->getPerfECM();
            double pred_time = 1/ECM_1;
            printf("temporal prediction: mlups = %f\n", 1/pred_time);

            INIT_TIME(stencil_);
            START_TIME(stencil_);
            stencil_1->run();
            STOP_TIME(stencil_);
            double time = GET_TIME(stencil_);
            double mlups = dim_x*dim_y*dim_z/time;
            printf("temporal: mlups = %f\n", mlups*1e-6);
        }

        //free stencil_1
        delete stencil_1;
    }

    for(int i=0; i<3; ++i)
    {
        free(files[i]);
    }
}

