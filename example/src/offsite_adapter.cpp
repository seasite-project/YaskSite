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
        __stencil->init(true);\
        __stencil->calcECM();\
        __stencil->printECM();\
        double ecm_mlups = __stencil->getPerfECM();\
        double ecm_cy_cl = freq*1000.0*cl/ecm_mlups;\
        printf("ECM performance = %f MLUP/s\n", ecm_mlups);\
        printf("ECM cycle prediction = %f cy/CL\n", ecm_cy_cl);\
        ecm_cy_cl_vec.push_back(ecm_cy_cl);\
    }\
    else if(strcmp(__mode,"BENCH")==0)\
    {\
        __stencil->init();\
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
        cy_cl_vec.push_back(cy_cl);\
    }\
    else if(strcmp(__mode,"VALIDATE")==0)\
    {\
        __stencil->init();\
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
        ecm_cy_cl_vec.push_back(ecm_cy_cl);\
        cy_cl_vec.push_back(cy_cl);\
    }\
    else\
    {\
        printf("Error: mode(-m) not recognized\n");\
    }\
}\

void print_vec(std::vector<double> vec, int thread_start, int thread_end, char* desc)
{
    if(vec.size()>0)
    {
        printf("%25s ||", "cores");
        for(int thread=thread_start; thread<thread_end; ++thread)
        {
            printf("%8d |", thread);
        }
        printf("%8d", thread_end);
        printf("\n");
        printf("%25s ||", desc);
        for(int thread=0; thread<(int)vec.size()-1; ++thread)
        {
            printf("%8.2f |", vec[thread]);
        }
        printf("%8.2f", vec[(int)vec.size()-1]);
    }
    printf("\n");
}

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

    os_parser optParse;
    optParse.parse_arg(argc, argv);

    char* mc_file = optParse.mcFile;

    //mc_file optional is passed to initialize with machine configuration
    MPI_Manager mpiMan(&argc, &argv, mc_file);


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

    std::vector<char*> threads_char;

    FILE *tmp;
    POPEN(NULL, tmp, "%s/threadPerSocket.sh %s", TOOL_DIR, glb_mc_file);
    int thread_per_socket = readIntVar(tmp);
    PCLOSE(tmp);

    if(optParse.cores == NULL)
    {
        int totCores = thread_per_socket;
        char *thread_str;
        STRINGIFY(thread_str, "1:%d", totCores);
        printf("thread_str = %s\n", thread_str);
        optParse.cores = thread_str;
    }
    threads_char = splitChar(optParse.cores);
    if(threads_char.size() == 0)
    {
        printf("Number of threads not provided\n");
    }
    int thread_start = atoi(threads_char[0]);
    int thread_end = thread_start;
    if(threads_char.size() == 2)
    {
        thread_end = atoi(threads_char[1]);
    }
    else if(threads_char.size() > 2)
    {
        printf("Thread string incompatible\n");
    }

    if(thread_end > thread_per_socket)
    {
        thread_end = thread_per_socket;
        printf("Only 1 ccNUMA domain supported, setting threads to range %d:%d\n", thread_start, thread_end);
    }

    int dt = optParse.iter;

    int radius = optParse.radius;

    bool prefetch = optParse.prefetch;
    bool dp = optParse.dp;

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
    yaskSite* stencil = new yaskSite(&mpiMan, derivedStencilName, dim, -1, fold[0], fold[1], fold[2], dp, prefetch);
    stencil->setDim(size[0], size[1], size[2]);

    std::vector<double> ecm_cy_cl_vec;
    std::vector<double> cy_cl_vec;
    for (int threads=thread_start; threads<=thread_end; ++threads)
    {
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
                RUN(stencil, mode);
            }
        }
    }

    printf("\nRESULTS\n\n");
    print_vec(ecm_cy_cl_vec, thread_start, thread_end, "ECM perf. (cy/CL)");
    printf("\n");
    print_vec(cy_cl_vec, thread_start, thread_end, "BENCHMARK perf. (cy/CL)");
}
