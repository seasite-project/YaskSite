#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "parse.h"
//#include "likwid.h"


#define PRINT_BANNER(file)\
{\
    FILE* writeFile;\
    writeFile = (file!=NULL) ?fopen(file,"a"):stdout;\
    fprintf(writeFile, "%6s, %3s, %5s, %5s, %5s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s, %8s\n", "Threads", "dt", "dx", "dy", "dz", "s", "mlups", "L2", "L3", "MEM", "ECMmlups", "ECML2", "ECML3", "ECMMEM", "Lat(%)", "Bound(%)", "Pref(%)", "Assoc(%)", "AT time(s)");\
    if(file!=NULL) fclose(writeFile);\
}

#define PERF_RUN(stencil, file, AT_time, preInit)\
{\
    if(preInit)\
    {\
        stencil->init();\
    }\
    FILE* writeFile;\
    writeFile = (file!=NULL) ?fopen(file,"a"):stdout;\
    INIT_TIME(stencil_);\
    bool validate = false;\
    stencil->calcECM(validate);\
    stencil->printECM();\
    START_TIME(stencil_);\
    double cur_time = 0;\
    double ctr=0;\
    while(cur_time<0.5)\
    {\
        stencil->run();\
        ctr++;\
        STOP_TIME(stencil_);\
        cur_time = GET_TIME(stencil_);\
    }\
    std::vector<double> ECM = stencil->getECM(true);\
    std::vector<double> ECM_validate(ECM.size(), -1);\
    double ECM_mlups = stencil->getPerfECM();\
    double dt=stencil->dt;\
    double dx=stencil->dx;\
    double dy=stencil->dy;\
    double dz=stencil->dz;\
    double mlups=ctr*dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    printf("AT time = %f\n", AT_time);\
    contribution ecm_contrib = stencil->getECMContributions();\
    fprintf(writeFile, "%6d, %3d, %5d, %5d, %5d, %8.5f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.2f, %8.4f, %8.4f, %8.4f, %8.4f, %8.5f\n", stencil->nthreads, stencil->dt, stencil->dx, stencil->dy, stencil->dz, time,mlups/time, ECM_validate[0], ECM_validate[1], ECM_validate[2], ECM_mlups, ECM[0], ECM[1], ECM[2], ecm_contrib.latency*100, ecm_contrib.boundary*100, ecm_contrib.prefetch*100, ecm_contrib.associativity*100, AT_time);\
    if(file!=NULL) fclose(writeFile);\
}\

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    //cpuFeatures_init();

    //generate code and specify in kernel i/p if needed
/*    CODIFY( "heat2d_update", "heat2d",
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
*/


    parser optParse;
    optParse.parse_arg(argc, argv);

    MPI_Manager mpiMan(&argc, &argv, optParse.mcFile);
    bool prefetch=optParse.prefetch;
    std::vector<char*> kernelFullDesc = splitChar(optParse.kernel);
    char* kernel = kernelFullDesc[0];
    int dimension = atoi(kernelFullDesc[1]);

    std::vector<int> range = getRange(optParse.innerDim);
    int start_dim = range[0];
    int end_dim = range[2];
    int inc_dim = range[1];

    std::vector<int> fold;
    if(strcmp(optParse.fold, "auto")==0)
    {
        int vecLen = 1;
        if(strcmp(INSTR, "AVX")==0)
        {
            vecLen = 4;
        }
        else if(strcmp(INSTR, "AVX512")==0)
        {
            vecLen = 8;
        }
        fold = {1,1,vecLen};
    }
    else
    {
        fold = getRange(optParse.fold);
    }


    int f_x = fold[0];
    int f_y = fold[1];
    int f_z = fold[2];

    int threads = optParse.cores;
    int dt = optParse.iter;

    int radius = optParse.radius;
    bool dp = optParse.dp;

    char* opt_range = optParse.opt;
    std::vector<char*> opt = splitChar(opt_range);
    std::vector<bool> opt_bool(4,false);

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
    if(findChar(opt,"AT")>=0)
    {
        opt_bool[3] = true;
    }


    char* outDir = optParse.outDir;
    std::vector<char*> files(4, NULL);
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
        if(opt_bool[3])
            STRINGIFY(files[3],"%s/AT.txt",file_path);

        free(file_path);
    }

    for(int i=0; i<=3; ++i)
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
        cache_info L3 = CACHE("L2"); //L3_cache macro defined by yaskSite library
        int L3_size = L3.bytes;
        double grid_size = 10*1024.0*1024.0*1024.0; //10 GB
        int dim_x=1, dim_y=1, dim_z=1;
        if(dimension == 3)
        {
            dim_x = static_cast<int>((grid_size)/((double)(4.0*8.0*dim*dim)));
            dim_x = static_cast<int>(dim_x/8.0) * 8; //make multiple of 8
            //dim_x = dim;
            dim_y = dim;//16;
            dim_z = dim;
        }
        else if(dimension==2)
        {
            dim_y = 250*threads;//static_cast<int>((200.0*L3_size)/((double)(2.0*8.0*dim)));
            dim_z = dim;
        }
        else
        {
            printf("Unknown stencil dimension\n");
        }

        yaskSite* stencil_1 = new yaskSite(&mpiMan, kernel, dimension, radius, f_z, f_y, f_x, dp, prefetch);
        stencil_1->setDim(dim_z, dim_y, dim_x, dt);
        stencil_1->setThread(threads,1);
        //stencil_1->init();//init goes here, warm-up


        INIT_TIME(AT);
        //plain run
        if(opt_bool[0])
        {
            START_TIME(AT);
            STOP_TIME(AT);
            double AT_time = GET_TIME(AT);
            PERF_RUN(stencil_1, files[0], AT_time, true);
        }

        //spatial blocked
        if(opt_bool[1])
        {
            START_TIME(AT);
            stencil_1->spatialTuner("L3", "L2",0.5, 0.5);
            STOP_TIME(AT);
            double AT_time = GET_TIME(AT);
            //stencil_1->setSubBlock(750,5,512);
            //stencil_1->setBlock(600,30,200);
            PERF_RUN(stencil_1, files[1], AT_time, true);
        }

        //temporal & spatial blocked
        if(opt_bool[2])
        {
            START_TIME(AT);
            if(f_y ==4 || f_x ==4 || strcmp(stencil_1->arch,"ivb")!=0)
            {
                stencil_1->blockTuner("L3","L2","L2", 0.5,0.5,0.5);
            }
            else
            {
                stencil_1->blockTuner("L3","L2","L1", 0.5,0.8,0.8);
            }
            STOP_TIME(AT);
            //stencil_1->ry = 15;
            double AT_time = GET_TIME(AT);
            PERF_RUN(stencil_1, files[2], AT_time, true);
        }

        if(opt_bool[3])
        {
            stencil_1->setBlock(32,32,32); //default block size to start as in yask.sh exe
            stencil_1->init();
            START_TIME(AT);
            stencil_1->setYASK_AT(true);
            STOP_TIME(AT);
            double AT_time = GET_TIME(AT);
            PERF_RUN(stencil_1, files[3], AT_time, false);
        }

        //free stencil_1
        delete stencil_1;


    }

    for(int i=0; i<=3; ++i)
    {
        free(files[i]);
    }


}

