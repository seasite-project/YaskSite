#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "parse.h"

#define COUPLE_GRID

//This measures the performance of different versions of heat2d
void main(int argc, char** argv)
{
    ////////////////Code for IRK method s=4/////////////////////////
    double A[4][4] = {
        {0.11299947932316, -0.04030922072352, 0.02580237742034, -0.0099046765073},
        {0.23438399574740, 0.20689257393536, -0.04785712804854, 0.01604742280652},
        {0.21668178462325, 0.40612326386737, 0.18903651817006, -0.02418210489983},
        {0.22046221117677, 0.38819346884317, 0.32884431998006, 1.0 / 16.0}
    };


    char *codeStr, *codeStr_cpy;
    STRINGIFY(codeStr,  "GRID_ARRAY F[4];\n"
            "GRID_POINT tmp;\n"
            "GRID_POINT_ARRAY tmp[4];\n"
            "PARAM dt;\n");
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
        STRINGIFY_ADD(codeStr, "F[%d]<0> = tmp[%d]*dt + DATA<0>;\n", l, l);
    }

    codeGen code1("heat2d_irk_1", "heat2d", codeStr);
    free(codeStr);

    STRINGIFY(codeStr,  "GRID_ARRAY F[4];\n"
            "GRID_POINT tmp;\n"
            "GRID_POINT_ARRAY tmp[4];\n"
            "PARAM dt;\n");
    for(int i=0; i<4; ++i)
    {
        STRINGIFY_ADD(codeStr,"tmp = STENCIL(F[%d]<0>);\n", i);
        for(int l=0; l<4; ++l)
        {
            STRINGIFY_ADD(codeStr, "tmp[%d] += %f*tmp;\n", l, A[l][i]);
        }
    }

    for(int l=0; l<4; ++l)
    {
        STRINGIFY_ADD(codeStr, "F[%d]<1> = tmp[%d]*dt + DATA<0>;\n", l, l);
    }

    codeGen code2("heat2d_irk_2", "heat2d", codeStr);


    free(codeStr);
    ////////////////////////////////////////////////////////////////////
    double b[4] = {0.22046221117677, 0.38819346884317, 0.32884431998006, 1.0 / 16.0};

    STRINGIFY(codeStr, "GRID_ARRAY F[4];\n"
            "GRID_POINT tmp;\n"
            "PARAM dt;\n"
            "tmp = %f*STENCIL(F[0]<0>);\n", b[0]);

    for(int l=1; l<4; ++l)
    {
        STRINGIFY_ADD(codeStr,"tmp = tmp +  %f*STENCIL(F[%d]<0>);\n", b[l], l);
    }

    STRINGIFY_ADD(codeStr, "DATA<1> = DATA<0> + dt*tmp;\n");
    codeGen code3("heat2d_irk_3", "heat2d", codeStr);
    free(codeStr);

    MPI_Manager mpiMan(&argc, &argv);

    parser optParse;
    optParse.parse_arg(argc, argv);

    bool prefetch=optParse.prefetch;
    //std::vector<char*> kernelFullDesc = splitChar(optParse.kernel);
    //char* kernel = kernelFullDesc[0];
    //int dimensiion = atoi(kernelFullDesc[1]);
    int dimension=2;

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

    //taking care size always fits only in memory
    //(here 200 times L3 is the total size)
    cache_info L3 = CACHE("L2"); //L3_cache macro defined by yaskSite library
    int L3_size = L3.bytes;

    yaskSite* stencil_1 = new yaskSite(&mpiMan, "heat2d_irk_1", dimension, radius, f_z, f_y, f_x, prefetch);
    stencil_1->setDim(dim_z, dim_y, dim_x, 1);
    stencil_1->setThread(threads,1);
    stencil_1->init();//init goes here, warm-up

    yaskSite* stencil_2 = new yaskSite(&mpiMan, "heat2d_irk_2", dimension, radius, f_z, f_y, f_x, prefetch);
    stencil_2->setDim(dim_z, dim_y, dim_x, 5);
    stencil_2->setThread(threads,1);
    stencil_2->init();//init goes here, warm-up

    yaskSite* stencil_3 = new yaskSite(&mpiMan, "heat2d_irk_3", dimension, radius, f_z, f_y, f_x, prefetch);
    stencil_3->setDim(dim_z, dim_y, dim_x, 1);
    stencil_3->setThread(threads,1);
    stencil_3->init();//init goes here, warm-up


#ifdef COUPLE_GRID
    for(int i=0; i<4; ++i)
    {
        char *grid_name;
        STRINGIFY(grid_name, "F_%d",i);

        (*stencil_1)[grid_name][0] << (*stencil_2)[grid_name][0];
        (*stencil_3)[grid_name][0] << (*stencil_2)[grid_name][1];
        free(grid_name);
    }
    (*stencil_1)["data"][0] << (*stencil_3)["data"][0];
    (*stencil_2)["data"][0] << (*stencil_3)["data"][0];
#endif

    double* h_inv = stencil_1->getParam("h_inv");
    (*h_inv) = (double)(dim_z-1.0)*(double)(dim_y-1.0);
    h_inv = stencil_2->getParam("h_inv");
    (*h_inv) = (double)(dim_z-1.0)*(double)(dim_y-1.0);
    h_inv = stencil_3->getParam("h_inv");
    (*h_inv) = (double)(dim_z-1.0)*(double)(dim_y-1.0);

    double* dt_param = stencil_1->getParam("dt");
    (*dt_param) = 1e-5*2;
    dt_param = stencil_2->getParam("dt");
    (*dt_param) = 1e-5*2;
    dt_param = stencil_3->getParam("dt");
    (*dt_param) = 1e-5*2;

    /*stencil_1->setInputScalar(0, "all", true);
    stencil_2->setInputScalar(0, "all", true);
    stencil_3->setInputScalar(0, "all", true);
    */

    //initialize all grids with 0
    for (int i = 0; i < dim_y; i++)
    {
        for (int j = 0; j < dim_z; j++)
        {
            double val = 0.5 + (double)j / (double)(dim_z - 1);
            stencil_1->setInputAt(val, "data", 0, i, j);
        }
    }


    for (int i = 0; i < dim_y; i++)
    {
        for (int j = 0; j < dim_z; j++)
        {
            double val = 0.5 + (double)j / (double)(dim_z - 1);
            stencil_1->setInputAt(val, "data", 0, i, j);
        }
    }

    stencil_1->write2dFile("iv.dat", "data", "y:z", 1);

    //plain run
    if(opt_bool[0])
    {
        stencil_1->calcECM(false);
        stencil_2->calcECM(false);
        stencil_3->calcECM(false);
        double ECM_1 = stencil_1->getPerfECM();
        double ECM_2 = stencil_2->getPerfECM();
        double ECM_3 = stencil_3->getPerfECM();
        double pred_time = 1/ECM_1 + 5/ECM_2 + 1/ECM_3;
        printf("plain prediction: mlups = %f\n", 1/pred_time);

        INIT_TIME(stencil_);
        START_TIME(stencil_);

        for(int ts=0; ts<dt; ++ts)
        {
            stencil_1->run();
            stencil_2->run();
            stencil_3->run();
        }

        STOP_TIME(stencil_);
        double time = GET_TIME(stencil_);
        double mlups = dim_x*dim_y*dim_z/time;
        printf("plain: mlups = %f\n", dt*mlups*1e-6);
    }

    //spatial blocked
    if(opt_bool[1])
    {
        stencil_1->spatialTuner("L3", "L2");
        stencil_2->spatialTuner("L3", "L2");
        stencil_3->spatialTuner("L3", "L2");

        stencil_1->calcECM(false);
        stencil_2->calcECM(false);
        stencil_3->calcECM(false);
        double ECM_1 = stencil_1->getPerfECM();
        double ECM_2 = stencil_2->getPerfECM();
        double ECM_3 = stencil_3->getPerfECM();
        double pred_time = 1/ECM_1 + 5/ECM_2 + 1/ECM_3;
        printf("spatial prediction: mlups = %f\n", 1/pred_time);

        INIT_TIME(stencil_);
        START_TIME(stencil_);
        for(int ts=0; ts<dt; ++ts)
        {
            stencil_1->run();
            stencil_2->run();
            stencil_3->run();
        }

        STOP_TIME(stencil_);
        double time = GET_TIME(stencil_);
        double mlups = dim_x*dim_y*dim_z/time;
        printf("spatial: mlups = %f\n", dt*mlups*1e-6);
    }

    //temporal & spatial blocked
    if(opt_bool[2])
    {
        stencil_1->spatialTuner("L3", "L2");
        stencil_2->blockTuner("L3","L3","L2", 0.5);
        stencil_3->spatialTuner("L3", "L2");

        stencil_1->calcECM(false);
        stencil_2->calcECM(false);
        stencil_3->calcECM(false);
        double ECM_1 = stencil_1->getPerfECM();
        double ECM_2 = stencil_2->getPerfECM();
        double ECM_3 = stencil_3->getPerfECM();
        double pred_time = 1/ECM_1 + 5/ECM_2 + 1/ECM_3;
        printf("temporal prediction: mlups = %f\n", 1/pred_time);

        INIT_TIME(stencil_);
        START_TIME(stencil_);
        for(int ts=0; ts<dt; ++ts)
        {
            stencil_1->run();
            stencil_2->run();
            stencil_3->run();
        }

        STOP_TIME(stencil_);
        double time = GET_TIME(stencil_);
        double mlups = dim_x*dim_y*dim_z/time;
        printf("temporal: mlups = %f\n", dt*mlups*1e-6);
    }

    stencil_3->write2dFile("solution.dat", "data", "y:z", 1);

    //free stencil_1
    delete stencil_1;
    delete stencil_2;
    delete stencil_3;

    for(int i=0; i<3; ++i)
    {
        free(files[i]);
    }
}

