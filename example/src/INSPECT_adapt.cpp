#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "timing.h"
#include "parse.h"
#include "likwid.h"

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


#define PRINT_BANNER(file)\
{\
    FILE* writeFile;\
    writeFile = (file!=NULL) ?fopen(file,"a"):stdout;\
    fprintf(writeFile, "N,dt,dx,dy,dz,rx,ry,rz,bx,by,bz,time,mlups,cy_cl,Tol,RegL1,L1L2,L2L3,L3Mem,L1L2_data,L2L3_data,L3Mem_data,ECM_mlups,ECM_cy_cl,ECM_Tol,ECM_RegL1,ECM_L1L2,ECM_L2L3,ECM_L3Mem,ECM_L1L2_data,ECM_L2L3_data,ECM_L3Mem_data,ECM_RegL1_base,ECM_L1L2_base,ECM_L2L3_base,ECM_L3Mem_base,ECM_L1L2_base_data,ECM_L2L3_base_data,ECM_L3Mem_base_data,ECM_L1L2_pre,ECM_L2L3_pre,ECM_L3Mem_pre,ECM_L1L2_pre_data,ECM_L2L3_pre_data,ECM_L3Mem_pre_data,ECM_L1L2_bound,ECM_L2L3_bound,ECM_L3Mem_bound,ECM_L1L2_bound_data,ECM_L2L3_bound_data,ECM_L3Mem_bound_data,ECM_L1L2_assoc,ECM_L2L3_assoc,ECM_L3Mem_assoc,ECM_L1L2_assoc_data,ECM_L2L3_assoc_data,ECM_L3Mem_assoc_data,ECM_RegL1_lat,ECM_L1L2_lat,ECM_L2L3_lat,ECM_L3Mem_lat,Burst_p,sat,realSat,effThreads\n");\
    if(file!=NULL) fclose(writeFile);\
}

double getNaive(yaskSite* stencil, int i)
{
    std::vector<double> ECM = stencil->getECM();
    std::vector<double> ECM_bound = stencil->getECM_boundary();
    std::vector<double> ECM_pre = stencil->getECM_prefetch();
    std::vector<double> ECM_assoc = stencil->getECM_assoc();

    double naive=0;
    naive = ECM[i]-(ECM_bound[i-1]+ECM_pre[i-1]+ECM_assoc[i-1]);

    return naive;
}


double getNaiveData(yaskSite* stencil, int i)
{
    std::vector<double> ECM_data = stencil->getECM(true);
    std::vector<double> ECM_bound_data = stencil->getECM_boundary(true);
    std::vector<double> ECM_pre_data = stencil->getECM_prefetch(true);
    std::vector<double> ECM_assoc_data = stencil->getECM_assoc(true);

    double naive=0;
    naive = ECM_data[i]-(ECM_bound_data[i+1]+ECM_pre_data[i+1]+ECM_assoc_data[i+1]);

    return naive;
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
    STOP_TIME(stencil_);\
    std::vector<double> ECM_data = stencil->getECM(true);\
    std::vector<double> ECM = stencil->getECM();\
    std::vector<double> ECM_bound_data = stencil->getECM_boundary(true);\
    std::vector<double> ECM_bound = stencil->getECM_boundary();\
    std::vector<double> ECM_pre_data = stencil->getECM_prefetch(true);\
    std::vector<double> ECM_pre = stencil->getECM_prefetch();\
    std::vector<double> ECM_assoc_data = stencil->getECM_assoc(true);\
    std::vector<double> ECM_assoc = stencil->getECM_assoc();\
    std::vector<double> ECM_latency = stencil->getECM_latency();\
    std::vector<double> ECM_validate_data = stencil->getECM_validate(true); \
    std::vector<double> ECM_validate = stencil->getECM_validate(); \
    /*add latency to final ECM*/\
    for(int i=0; i<ECM_latency.size(); ++i)\
    {\
        ECM[i+1] += ECM_latency[i];\
    }\
    std::vector<double> sat = stencil->getSaturation();\
    double burst_penalty = stencil->getECM_BurstPenalty();\
    double ECM_mlups = stencil->getPerfECM();\
    double dt=stencil->dt;\
    double dx=stencil->dx;\
    double dy=stencil->dy;\
    double dz=stencil->dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    double freq=stencil->getCpuFreq();\
    double cl=CACHE("MEM").cl_size/(double)CACHE("MEM").bytePerWord;\
    contribution ecm_contrib = stencil->getECMContributions();\
    /*    fprintf(writeFile,"%6d,%4d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%8.5f,%8.2f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.2f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f\n", stencil->dx, stencil->dt, stencil->dx, stencil->dy, stencil->dz, stencil->rx, stencil->ry, stencil->rz, stencil->bx, stencil->by, stencil->bz, time, mlups/time, cl*time*freq*1000/mlups, ECM_validate[0]*cl, ECM_validate[1]*cl, ECM_validate[2]*cl, ECM_validate[3]*cl, ECM_validate[4]*cl, ECM_validate_data[0], ECM_validate_data[1], ECM_validate_data[2], ECM_mlups, freq*1000.0*cl/ECM_mlups, ECM[0]*cl, ECM[1]*cl, ECM[2]*cl, ECM[3]*cl, ECM[4]*cl, ECM_data[0], ECM_data[1], ECM_data[2], ECM_pre[1]*cl, ECM_pre[2]*cl, ECM_pre[3]*cl, ECM_pre_data[1], ECM_pre_data[2], ECM_pre_data[3], ECM_bound[1], ECM_bound[2], ECM_bound[3], ECM_bound_data[1], ECM_bound_data[2], ECM_bound_data[3], ECM_assoc[1], ECM_assoc[2], ECM_assoc[3], ECM_assoc_data[1], ECM_assoc_data[2], ECM_assoc_data[3], ECM_latency[0], ECM_latency[1], ECM_latency[2], ECM_latency[3], burst_penalty);\*/\
    /*Note ECM is always scaled for single core*/\
    fprintf(writeFile, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", stencil->dz, stencil->dt, stencil->dx, stencil->dy, stencil->dz, stencil->rx, stencil->ry, stencil->rz, stencil->bx, stencil->by, stencil->bz, time, mlups/time, sat[0]*cl*time*freq*1000/mlups, ECM_validate[0]*cl, ECM_validate[1]*cl, ECM_validate[2]*cl, ECM_validate[3]*cl, ECM_validate[4]*cl, ECM_validate_data[0], ECM_validate_data[1], ECM_validate_data[2], ECM_mlups, sat[0]*freq*1000.0*cl/ECM_mlups, ECM[0]*cl, ECM[1]*cl, ECM[2]*cl, ECM[3]*cl, ECM[4]*cl, ECM_data[0], ECM_data[1], ECM_data[2], getNaive(stencil,1)*cl, getNaive(stencil,2)*cl, getNaive(stencil,3)*cl, getNaive(stencil,4)*cl, getNaiveData(stencil,0), getNaiveData(stencil,1), getNaiveData(stencil,2), ECM_pre[1]*cl, ECM_pre[2]*cl, ECM_pre[3]*cl, ECM_pre_data[1], ECM_pre_data[2], ECM_pre_data[3], ECM_bound[1]*cl, ECM_bound[2]*cl, ECM_bound[3]*cl, ECM_bound_data[1], ECM_bound_data[2], ECM_bound_data[3], ECM_assoc[1]*cl, ECM_assoc[2]*cl, ECM_assoc[3]*cl, ECM_assoc_data[1], ECM_assoc_data[2], ECM_assoc_data[3], ECM_latency[0]*cl, ECM_latency[1]*cl, ECM_latency[2]*cl, ECM_latency[3]*cl, burst_penalty, sat[0], sat[1], sat[2]);\
    if(file!=NULL) fclose(writeFile);\
}\

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    cpuFeatures_init();

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
        STRINGIFY(file_path, "%s", outDir);
        //, kernel, radius, (prefetch)?"prefetch":"no_prefetch", optParse.path, f_x, f_y, f_z);

        //create directories if not present
        char *mkdirCmd;
        STRINGIFY(mkdirCmd, "mkdir -p %s", file_path);
        system(mkdirCmd);
        free(mkdirCmd);

        if(opt_bool[0])
            STRINGIFY(files[0],"%s/plain.csv",file_path);
        if(opt_bool[1])
            STRINGIFY(files[1],"%s/spatial.csv",file_path);
        if(opt_bool[2])
            STRINGIFY(files[2],"%s/temporal.csv",file_path);

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
        for(int i=0; i<3; ++i)
        {
            std::vector<bool> exec_bool(3,false);
            if(opt_bool[i] == true)
            {
                exec_bool[i] = opt_bool[i];
                //taking care size always fits only in memory
                //(here 200 times L3 is the total size)
                cache_info L3 = CACHE("L2"); //L3_cache macro defined by yaskSite library
                int L3_size = L3.bytes;
                int dim_x=1, dim_y=1, dim_z=1;
                if(dimension == 3)
                {
                    dim_x = 200;//static_cast<int>((200.0*L3_size)/((double)(2.0*8.0*dim*dim)));
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

                yaskSite* stencil_1 = new yaskSite(&mpiMan, kernel, dimension, radius, f_z, f_y, f_x, prefetch);
                stencil_1->setDim(dim_z, dim_y, dim_x, dt);
                stencil_1->setThread(threads,1);

                /*
                   stencil_1->init();//init goes here, warm-up
                   double* hinv = stencil_1->getParam("h_inv");
                   (*hinv) = 1/((double)dim*(double)dim*(double)dim);
                   */
                printf("GENERATE_DIR : %s\n", stencil_1->localDir);
                //DISABLE_PREFETCHER(threads);


                //plain run
                if(exec_bool[0])
                {
                    //    PERF_RUN(stencil_1, files[0]);
                }
                //spatial blocked
                if(exec_bool[1])
                {
                    /*            if( strcmp(stencil_1->arch,"skx")==0 )
                                  {
                                  stencil_1->spatialTuner("L2", "L2",0.5, 0.5);
                                  }
                                  else*/
                    {
                        stencil_1->spatialTuner("L3", "L2",0.5, 0.5);
                    }
                    //stencil_1->setSubBlock(750,5,512);
                }
                //temporal & spatial blocked
                if(exec_bool[2])
                {
                    /*            if(f_y ==4 || f_x ==4 || strcmp(stencil_1->arch,"ivb")!=0)
                                  {
                                  printf("Wrong Am I here.......\n");
                                  stencil_1->blockTuner("L3","L2","L2", 0.5,0.5,0.5);
                                  }
                                  else*/
                    {
                        //stencil_1->blockTuner("L3","L2","L1", 0.16,0.65,0.65);

                         stencil_1->blockTuner("L3","L2","L1", 0.36,0.8,0.8);

                        //                stencil_1->setBlock(600,16,8);

                        //                stencil_1->setSubBlock(600,16,8);
                        /*                stencil_1->setRegion(1200,16,39,10,false);
                                          stencil_1->setBlock(400,8,13);
                                          stencil_1->setSubBlock(400,8,13);*/
                        // stencil_1->blockTuner("L3","L3","L2", 0.5,0.8,0.8);
                    }
                }
                stencil_1->init();//init goes here, warm-up
/*                double* hinv = stencil_1->getParam("h_inv");
                (*hinv) = 1/((double)dim*(double)dim*(double)dim);
*/
                char *currFile = files[i];
                PERF_RUN(stencil_1, currFile);


                //ENABLE_PREFETCHER(threads);

                //free stencil_1
                delete stencil_1;
            }
        }
    }

    for(int i=0; i<3; ++i)
    {
        free(files[i]);
    }


}

