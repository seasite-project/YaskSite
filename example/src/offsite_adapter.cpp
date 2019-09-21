#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "offsite_parse.h"

#define COUPLE_GRID

#define RUN(__stencil, __mode)\
{\
    double freq = __stencil->getCpuFreq();\
    double cl = CACHE("MEM").cl_size/(double)CACHE("MEM").bytePerWord;\
    if(strcmp(__mode,"ECM")==0)\
    {\
        __stencil->calcECM();\
        __stencil->printECM();\
        double ecm_mlups = __stencil->getPerfECM();\
        double cy_cl = freq*1000.0*cl/ecm_mlups;\
        printf("ECM performance = %f MLUP/s\n", ecm_mlups);\
        printf("ECM cycle prediction = %f cy/CL\n", cy_cl);\
    }\
    else if(strcmp(__mode,"BENCH")==0)\
    {\
        INIT_TIME(stencil_run);\
        START_TIME(stencil_run);\
        __stencil->run();\
        STOP_TIME(stencil_run);\
        double time = GET_TIME(stencil_run);\
        double mlups = (double)__stencil->dt*(double)__stencil->dx*(double)__stencil->dy*(double)__stencil->dz;\
        double cy_cl = freq*1e9*cl*time/mlups;\
        printf("Benchmark performance = %f MLUP/s\n", mlups*1e-6/time);\
        printf("Benchamrk time = %f s\n", time);\
        printf("Benchmark cycle prediction = %f cy/CL\n", cy_cl);\
    }\
    else if(strcmp(__mode,"VALIDATE")==0)\
    {\
        __stencil->calcECM(true);\
        __stencil->printECM();\
        double ecm_mlups = __stencil->getPerfECM();\
        double ecm_cy_cl = freq*1000*cl/ecm_mlups;\
        printf("ECM performance = %f MLUP/s\n", ecm_mlups);\
        printf("ECM cycle prediction = %f cy/CL\n", ecm_cy_cl);\
        INIT_TIME(stencil_run);\
        START_TIME(stencil_run);\
        __stencil->run();\
        STOP_TIME(stencil_run);\
        double time = GET_TIME(stencil_run);\
        double mlups = (double)__stencil->dt*(double)__stencil->dx*(double)__stencil->dy*(double)__stencil->dz;\
        double cy_cl = freq*1e9*cl*time/mlups;\
        printf("Benchmark performance = %f MLUP/s\n", mlups*1e-6/time);\
        printf("Benchamrk time = %f s\n", time);\
        printf("Benchmark cycle prediction = %f cy/CL\n", cy_cl);\
    }\
    else\
    {\
        printf("Error: mode(-m) not recognized\n");\
    }\
}\


//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    if(argc < 5)
    {
        printf("Usage : %s [code file] [key] [stencilParam] [stencilName] [optArgs]\n", argv[0]);
        exit(-1);
    }

    char *codeFile = argv[1];
    char *derivedStencilName = argv[2];
    char *stencilParams = argv[3];
    char *stencilName = argv[4];

    char * codeStr = NULL;
    long length;
    FILE * f = fopen (codeFile, "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        codeStr = (char*)malloc ((length+1)*sizeof(char));
        memset( codeStr, 0, sizeof(codeStr) );
        if (codeStr)
        {
            fread(codeStr, sizeof(char), length+1, f);
        }
        codeStr[length] = '\0';
        fclose (f);
    }

    if(codeStr == NULL)
    {
        printf("Couldn't read code file\n");
        exit(-1);
    }

    printf("code = %s", codeStr);

    codeGen code(derivedStencilName, stencilName, codeStr);

    free(codeStr);

    MPI_Manager mpiMan(&argc, &argv);

    os_parser optParse;
    optParse.parse_arg(argc, argv);

    std::vector<int> size = getRange(optParse.size);
    int dim = (int)size.size();

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
        fold = {vecLen,1,1};
    }
    else
    {
        fold = getRange(optParse.fold);
    }

    int threads = optParse.cores;

    int dt = optParse.iter;

    int radius = optParse.radius;

    bool prefetch = optParse.prefetch;

    char* mode = optParse.mode;

    std::vector<char*> opt = splitChar(optParse.opt);
    std::vector<bool> opt_bool(3, false);

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

    printf("dim = %d\n", dim);

    printf("stencilName = %s\n", stencilName);
    yaskSite* stencil = new yaskSite(&mpiMan, derivedStencilName, dim, -1, fold[0], fold[1], fold[2], prefetch);
    stencil->setDim(size[0], size[1], size[2]);
    stencil->setThread(threads, 1);

    for(int opt_idx=0; opt_idx<2; ++opt_idx)
    {
        if(opt_bool[opt_idx])
        {
            if(opt_idx == 1)
            {
                stencil->spatialTuner("L3", "L2", 0.5, 0.5);
            }
            else if(opt_idx == 2)
            {
                stencil->blockTuner("L3","L2","L1", 0.5,0.5,0.5);
            }
            stencil->init();
            RUN(stencil, mode);
        }
    }
}
