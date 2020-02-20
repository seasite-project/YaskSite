#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "offsite_parse.h"
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#define COUPLE_GRID

#define RUN(__stencil, __mode, push)\
{\
    double freq = __stencil->getCpuFreq();\
    double cl = CACHE("MEM").cl_size/(double)CACHE("MEM").bytePerWord;\
    ecm_cy_cl = -1;\
    cy_cl = -1;\
    if(strcmp(__mode,"ECM")==0)\
    {\
        __stencil->init();\
        __stencil->calcECM();\
        __stencil->printECM();\
        double ecm_mlups = __stencil->getPerfECM();\
        ecm_cy_cl = freq*1000.0*cl/ecm_mlups;\
        printf("ECM performance = %f MLUP/s\n", ecm_mlups);\
        printf("ECM cycle prediction = %f cy/CL\n", ecm_cy_cl);\
        if(push)\
        {\
            ecm_cy_cl_vec.push_back(ecm_cy_cl);\
        }\
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
        cy_cl = freq*1e9*cl*time/mlups;\
        printf("Benchmark performance = %f MLUP/s\n", mlups*1e-6/time);\
        printf("Benchamrk time = %f s\n", time);\
        printf("Benchmark cycle prediction = %f cy/CL\n", cy_cl);\
        if(push)\
        {\
            cy_cl_vec.push_back(cy_cl);\
        }\
    }\
    else if(strcmp(__mode,"VALIDATE")==0)\
    {\
        __stencil->init();\
        __stencil->calcECM(true);\
        printf("ECM finished\n");\
        __stencil->printECM();\
        double ecm_mlups = __stencil->getPerfECM();\
        ecm_cy_cl = freq*1000*cl/ecm_mlups;\
        printf("ECM performance = %f MLUP/s\n", ecm_mlups);\
        printf("ECM cycle prediction = %f cy/CL\n", ecm_cy_cl);\
        INIT_TIME(stencil_run);\
        START_TIME(stencil_run);\
        __stencil->run();\
        STOP_TIME(stencil_run);\
        double time = GET_TIME(stencil_run);\
        double mlups = (double)__stencil->dt*(double)__stencil->dx*(double)__stencil->dy*(double)__stencil->dz;\
        cy_cl = freq*1e9*cl*time/mlups;\
        printf("Benchmark performance = %f MLUP/s\n", mlups*1e-6/time);\
        printf("Benchamrk time = %f s\n", time);\
        printf("Benchmark cycle prediction = %f cy/CL\n", cy_cl);\
        if(push)\
        {\
            ecm_cy_cl_vec.push_back(ecm_cy_cl);\
            cy_cl_vec.push_back(cy_cl);\
        }\
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

double interpolate(int x,int x1, double y1, int x2, double y2)
{
    //y=y1+(x-x1)*m
    double m = (y2-y1)/((double)(x2-x1));
    return (y1+(x-x1)*m);
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
    POPEN(NULL, tmp, "%s/threadPerNUMA.sh %s", TOOL_DIR, glb_mc_file);
    int thread_per_socket = readIntVar(tmp);
    PCLOSE(tmp);
    POPEN(NULL, tmp, "%s/numaPerSocket.sh %s", TOOL_DIR, glb_mc_file);
    int numa_per_socket = readIntVar(tmp);
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

    int inter_node_thread = thread_end+1;
    if(thread_end > thread_per_socket)
    {
        inter_node_thread = thread_per_socket + 1;
        //thread_end = thread_per_socket;
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
    stencil->setDim(size[0], size[1], size[2], dt);

    std::vector<double> ecm_cy_cl_vec;
    std::vector<double> cy_cl_vec;
    for (int threads=thread_start; threads<inter_node_thread; ++threads)
    {
        stencil->setThread(threads, 1);

        for(int opt_idx=0; opt_idx<2; ++opt_idx)
        {
            if(opt_bool[opt_idx])
            {
                if(opt_idx == 1)
                {
                    stencil->spatialTuner("L2", "L2", 0.5, 0.5);
                }
                else if(opt_idx == 2)
                {
                    stencil->blockTuner("L3","L2","L1", 0.5,0.5,0.5);
                }
                double ecm_cy_cl, cy_cl;
                RUN(stencil, mode, true);
            }
        }
    }

    if(thread_end > inter_node_thread)
    {
        //cacuate performance at 1 NUMA node
        stencil->setThread(thread_per_socket, 1);
        double ecm_cy_cl, cy_cl;
        for(int opt_idx=0; opt_idx<2; ++opt_idx)
        {
            if(opt_bool[opt_idx])
            {
                if(opt_idx == 1)
                {
                    stencil->spatialTuner("L2", "L2", 0.5, 0.5);
                }
                else if(opt_idx == 2)
                {
                    stencil->blockTuner("L3","L2","L1", 0.5,0.5,0.5);
                }
                RUN(stencil, mode, true);
            }
        }

        for (int threads=inter_node_thread; threads<=thread_end; ++threads)
        {
            //Do a linear plot, will work if each ccNUMA has no shared resources
            //like L3
            if(ecm_cy_cl > 0)
            {
                double oneNumaPerf = 1.0/ecm_cy_cl;
                double allNumaPerf = numa_per_socket*oneNumaPerf*0.78; //0.78 is the efficiency of AMD ROME, currently a hack: it is 1 for Intel arch
                double ecm_perf = interpolate(threads, thread_per_socket, oneNumaPerf, thread_per_socket*numa_per_socket, allNumaPerf);
                ecm_cy_cl_vec.push_back(1.0/ecm_perf);
            }
            if(cy_cl > 0)
            {
                //this means run mode is active
                stencil->setThread(threads, 1);
                for(int opt_idx=0; opt_idx<2; ++opt_idx)
                {
                    if(opt_bool[opt_idx])
                    {
                        if(opt_idx == 1)
                        {
                            stencil->spatialTuner("L2", "L2", 0.5, 0.5);
                        }
                        else if(opt_idx == 2)
                        {
                            stencil->blockTuner("L3","L2","L1", 0.5,0.5,0.5);
                        }
                        RUN(stencil, mode, true);
                    }
                }
                cy_cl_vec.push_back(cy_cl);
            }
        }
    }

    printf("\nRESULTS\n\n");
    print_vec(ecm_cy_cl_vec, thread_start, thread_end, "ECM perf. (cy/CL)");
    printf("\n");
    print_vec(cy_cl_vec, thread_start, thread_end, "BENCHMARK perf. (cy/CL)");
}
