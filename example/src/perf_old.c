#include <yaskSite.h>
#include <stdio.h>
#include <stdlib.h>
#include "timing.h"


#define PRINT_BANNER(file)\
{\
    FILE* writeFile;\
    writeFile = fopen(file,"a");\
    fprintf(writeFile, "%3s\t %5s\t %5s\t %5s\t %9s\t %9s\n", "dt", "dx", "dy", "dz",  "mlups", "s");\
    fclose(writeFile);\
}

#define PERF_RUN(stencil, file)\
{\
    FILE* writeFile;\
    writeFile = fopen(file,"a");\
    INIT_TIME(stencil_);\
    START_TIME(stencil_);\
    stencil->run();\
    STOP_TIME(stencil_);\
    double dt=stencil->dt;\
    double dx=stencil->dx;\
    double dy=stencil->dy;\
    double dz=stencil->dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    fprintf(writeFile, "%3d\t %5d\t %5d\t %5d\t %5.4f\t %3.6f\n", stencil->dt, stencil->dx, stencil->dy, stencil->dz,  mlups/time, time);\
    fclose(writeFile);\
}\

//This measures the performance of different versions of heat3d
void main(int argc, char** argv)
{
    MPI_Manager mpiMan(&argc, &argv);

    //loop on prefetch
    for(int pre=0; pre<=1; ++pre)
    {
        bool prefetch=pre;
        int start_dim = 40;
        int end_dim = 1300;
        int inc_dim = 20;

        //for default path
        char* default_path;
        STRINGIFY(default_path, "perf_results/%s/default", (prefetch)?"prefetch":"no_prefetch");
        char* serpentine_path;
        STRINGIFY(serpentine_path, "perf_results/%s/serpentine", (prefetch)?"prefetch":"no_prefetch");

        //create directories if not present
        char *mkdirCmd;
        STRINGIFY(mkdirCmd, "mkdir -p %s", default_path);
        system(mkdirCmd);
        free(mkdirCmd);

        STRINGIFY(mkdirCmd, "mkdir -p %s", serpentine_path);
        system(mkdirCmd);
        free(mkdirCmd);

        std::vector<char*> files(7);

        STRINGIFY(files[0],"%s/plain.txt",default_path);
        STRINGIFY(files[1],"%s/spatial.txt",default_path);
        STRINGIFY(files[2],"%s/temporal.txt",default_path);

        STRINGIFY(files[3],"%s/plain.txt",serpentine_path);
        STRINGIFY(files[4],"%s/spatial_1.txt",serpentine_path);
        STRINGIFY(files[5],"%s/spatial_2.txt",serpentine_path);
        STRINGIFY(files[6],"%s/temporal.txt",serpentine_path);

        free(default_path);
        free(serpentine_path);

        for(int i=0; i<7; ++i)
        {
            PRINT_BANNER(files[i]);
        }

        for(int dim=start_dim; dim<end_dim; dim+=inc_dim)
        {
            //taking care size always fits only in memory
            //(here 200 times L3 is the total size)
            cache_info L3(L3_cache); //L3_cache macro defined by yaskSite library
            int L3_size = L3.bytes;
            int dim_outer = static_cast<int>((200.0*L3_size)/((double)(2.0*8.0*dim*dim)));

            yaskSite* stencil_1 = new yaskSite(&mpiMan, "heat3d", 3, 1,1,1, prefetch);
            stencil_1->setDim(dim, dim, dim_outer, 50);
            stencil_1->setThread(10,1);
            stencil_1->run();//init goes here, warm-up

            //plain run
            PERF_RUN(stencil_1, files[0]);

            //spatial blocked
            stencil_1->spatialTuner("L3", "L2");
            PERF_RUN(stencil_1, files[1]);

            //temporal & spatial blocked
            stencil_1->blockTuner("L3","L3","L2");
            PERF_RUN(stencil_1, files[2]);

            //free stencil_1
            delete stencil_1;

            //change path
            yaskSite* stencil_2 = new yaskSite(&mpiMan, "heat3d", 3, 1, 1, 1, "square_wave serpentine");
            stencil_2->setDim(dim, dim, dim_outer, 50);
            stencil_2->setThread(10,1);
            stencil_2->run();//warm-up

            //plain run
            PERF_RUN(stencil_2, files[3]);

            //spatial blocking
            stencil_2->spatialTuner("L3","L2");
            PERF_RUN(stencil_2, files[4]);

            //reduce safety factor; since serpentine requires
            //more layer in cache
            stencil_2->spatialTuner("L3","L2",0.25,0.25);
            PERF_RUN(stencil_2, files[5]);

            //temporal & spatial blocked
            stencil_2->blockTuner("L3", "L3", "L2",0.25,0.25,0.25);
            PERF_RUN(stencil_2, files[6]);

            //free stencil_2
            delete stencil_2;
        }

        for(int i=0; i<7; ++i)
        {
            free(files[i]);
        }
    }
}
