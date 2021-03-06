#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include "yaskSite.h"
#include "config.h"
#include "print.h"
#include "util.h"
#include "yaskFork.h"
#include <omp.h>
#include <math.h>
#include <algorithm>
#include "perfModel.h"
#include "macros.h"
#include <iostream>
#include <algorithm>
#include <iterator>

#ifdef ENABLE_LOOPADAPT
#include <loop_adapt.h>
#endif

#ifdef LIKWID_PERFMON
#include <likwid.h>
#endif

//stencil_arch_foldz_foldy_foldx_path_likwid
#define LOCAL_DIR_NAME\
    "%s/%s_%s_%s_%d_%d_%d_%s_%s_%s_%s", currPath, stencil, radius_str, arch, fold_z, fold_y, fold_x, (dp)?"dp":"sp", (prefetch)?"pre_on":"pre_off",(strcmp(path_wo_space,"")==0)?"default":path_wo_space,(likwid)?"likwid":"noLikwid"

yaskSite::yaskSite(MPI_Manager* mpi_man_, STENCIL* stencil_):stencilDetails(stencil_),overallModel(NULL, 0)
{
    STENCIL_struct_alloc = false;
    initStencil(mpi_man_, stencilDetails->name, stencilDetails->dim, stencilDetails->radius, stencilDetails->fold_z, stencilDetails->fold_y, stencilDetails->fold_x, stencilDetails->dp, stencilDetails->prefetch, stencilDetails->path, false);
}

yaskSite::yaskSite(MPI_Manager* mpi_man_, char* stencilName_, int dim_, int radius_, int fold_z_, int fold_y_, int fold_x_, bool dp_, bool prefetch_, char* path_, bool likwid):stencilDetails(NULL),overallModel(NULL,0)
{
    STENCIL_struct_alloc = true;
    stencilDetails = new STENCIL;
    stencilDetails->name = stencilName_;
    stencilDetails->dim = dim_;
    stencilDetails->radius = radius_;
    stencilDetails->fold_z = fold_z_;
    stencilDetails->fold_y = fold_y_;
    stencilDetails->fold_x = fold_x_;
    stencilDetails->prefetch = prefetch_;
    stencilDetails->path = path_;
    stencilDetails->dp = dp;
    initStencil(mpi_man_, stencilName_, dim_, radius_, fold_z_, fold_y_, fold_x_, dp_, prefetch_, path_, likwid);
}

void yaskSite::initStencil(MPI_Manager* mpi_man_, char* stencilName_, int dim_, int radius_, int fold_z_, int fold_y_, int fold_x_, bool dp_, bool prefetch_, char* path_, bool likwid)/*:mpi_man(mpi_man_), stencil(stencil_), arch(TARGET_ARCH), yaskDir(YASK_PATH), stencilCode(NULL), localDir(NULL), sysLogFileName(NULL), nthreads(1), threadPerBlock(1), fold_x(fold_x_), fold_y(fold_y_), fold_z(fold_z_), sbx(-1), sby(-1), sbz(-1), bx(-1), by(-1), bz(-1), rt(-1), rx(-1), ry(-1), rz(-1), dt(-1), dx(-1), dy(-1), dz(-1), s(-1), totalTime(0), radius(radius_), dim(dim_), buildWithLikwid(likwid), path(path_), stencilReady(false), subBlockUpdated(false), blockUpdated(false), regionUpdated(false), dimUpdated(false), threadUpdated(false), onceRun(false), prefetch(prefetch_), cacheStale(false), dl_handle(NULL), dynInit(&YASKinit), dynFinalize(&YASKfinalize), dynStencil(&YASKstencil), dynGetElement(&YASKgetElement), stencilFactory(NULL), stencilSoln(NULL)*/
{
    /*INIT START*/
    mpi_man = mpi_man_;
    stencil = stencilName_;
    arch = TARGET_ARCH;
    yaskDir = YASK_PATH;
    stencilCode = NULL;
    localDir = NULL;
    sysLogFileName = NULL;
    nthreads = 1;
    threadPerBlock = 1;
    fold_x = fold_x_;
    fold_y = fold_y_;
    fold_z = fold_z_;
    dp = dp_;
    sbx = -1;
    sby = -1;
    sbz = -1;
    bx = -1;
    by = -1;
    bz = -1;
    rt = -1;
    rx = -1;
    ry = -1;
    rz = -1;
    dt = -1;
    dx = -1;
    dy = -1;
    dz = -1;
    bx_min = 4;
    by_min = 4;
    bz_min = 32;
    s = -1;
    maxNumStencils = -1;
    totalTime = 0;
    radius = radius_;
    dim = dim_;
    buildWithLikwid = likwid;
    path = path_;
    stencilReady = false;
    subBlockUpdated = false;
    blockUpdated = false;
    regionUpdated = false;
    dimUpdated = false;
    threadUpdated = false;
    onceRun = false;
    prefetch = prefetch_;
    cacheStale = false;
    dl_handle = NULL;
    cpu_freq = 0;

    dynInit = &YASKinit;
    dynFinalize = &YASKfinalize;
    dynStencil = &YASKstencil;
    dynGetElement = &YASKgetElement;
    dynSetElement = &YASKsetElement;
    dynSetAll = &YASKsetAll;
    dynGetGridPtr = &YASKgetGridPtr;
    dynFuseGrid = &YASKfuseGrid;
    dynGetHalo = &YASKgetHalo;
    dynYASK_AT = &YASKautoTuner;
    //dynSetHalo = YASKsetHalo;

    stencilFactory = NULL;
    stencilSoln = NULL;

    if(dp)
    {
        updateBytePerWord(sizeof(double));
    }
    else
    {
        updateBytePerWord(sizeof(float));
    }

    /* INIT END */

    //this is the path of scratch directory
    char* currPath = TEMP_DIR; //get_current_dir_name();

    //remove space in path and replace by underScore
    char* path_wo_space = removeSpaces(path);
    //sync now itself to get the name correct
    SYNC_WITH_DIM(fold_z, fold_y, fold_x, "fold");

    char* radius_str;

    FILE* isRadiusFile;
    //check whether we have radius defined stencils
    POPEN(sysLogFileName, isRadiusFile, "%s/codeGenerator/isStencilRadius.sh %s %s %s", TOOL_DIR, stencil, STENCIL_RAW_DIR, TOOL_DIR);
    int isRadius = readIntVar(isRadiusFile);
    PCLOSE(isRadiusFile);

    //if no radius
    if(!isRadius)
    {
        radius = -1;
        STRINGIFY(radius_str, "no_radius");
    }
    else
    {
        STRINGIFY(radius_str, "radius_%d", radius);
    }

    STRINGIFY(localDir, LOCAL_DIR_NAME); //localDir is the code-generated directory

    delete[] path_wo_space;

    SYSTEM(sysLogFileName, "mkdir -p %s/bin", localDir);
    SYSTEM(sysLogFileName, "cp -r %s/stencils %s/.", currPath, localDir);
    //move the generated stencil, if this are extra stencils
    for(int i=0; i<(int)extraStencils.size(); ++i)
    {
        PRINT_LOG("user defined stencils = %s\n", extraStencils[i].c_str());
        SYSTEM(sysLogFileName, "mv  %s/stencils_gen/%s.* %s/stencils/.", currPath, extraStencils[i].c_str(), localDir);
    }
    STRINGIFY(sysLogFileName, "%s/sysCallLog.log", localDir);

    //stencil code
    STRINGIFY(stencilCode,"stencil_%s_%s_%s_%d_%d_%d_%s_%s_%s", stencil, radius_str, arch, fold_x, fold_y, fold_z, (dp)?"dp":"sp", (prefetch)?"pre_on":"pre_off", (strcmp(path,"")==0)?"default":path);

    delete[] radius_str;

    //check if the existing cache is new or old
    FILE* cacheCheckFile;
    POPEN(sysLogFileName, cacheCheckFile, "%s/codeGenerator/needCacheUpdate.sh %s %s %s %s", TOOL_DIR, stencil, STENCIL_RAW_DIR, localDir, TOOL_DIR);
    cacheStale=readIntVar(cacheCheckFile);
    PCLOSE(cacheCheckFile);

    //build stencil
    build();

   //get radius, and s from stencil_code
    FILE *radFile;

    //automatically detect radius if not specified
    if(radius==-1)
    {
        POPEN(sysLogFileName, radFile, "%s/getRadius.sh %s/build/stencil_code.hpp", TOOL_DIR, localDir);
        radius=readIntVar(radFile);
        PCLOSE(radFile);
    }

    //Read grid details
    char *gridFile;
    STRINGIFY(gridFile, "%s/gridFile.txt",localDir);
    if(mpi_man->myRank == 0) // only one rank as file writes occur
    {
        SYSTEM(sysLogFileName, "%s/getGrids.sh %s/build_out.txt %s", TOOL_DIR, localDir, gridFile);
    }
    mpi_man->global_barrier();
    std::vector<std::string> grid_name;
    std::vector<int> grid_dim;
    readTable(gridFile, grid_name, 0, -1, 0, 0);
    readTable(gridFile, grid_dim, 0, -1, 1, 1);
    free(gridFile);
    for(int i=0; i<(int)grid_name.size(); ++i)
    {
        GRID curr_grid(i, grid_name[i], grid_dim[i]);
        gridMap[grid_name[i]] = i;

        char* haloFile;
        STRINGIFY(haloFile, "%s/haloFile.txt",localDir);

        if(mpi_man->myRank == 0) // only one rank as file writes occur
        {
            SYSTEM(sysLogFileName, "%s/getHalo.sh %s %s/build/stencil_code.hpp > %s", TOOL_DIR, grid_name[i].c_str(), localDir, haloFile);
        }
        mpi_man->global_barrier();
        readTable(haloFile, curr_grid.halo, 0, -1, 0, 0);
        free(haloFile);
        FILE* tsFile;

        POPEN(sysLogFileName, tsFile, "%s/getTimeSteps.sh %s %s/build/stencil_code.hpp", TOOL_DIR, grid_name[i].c_str(), localDir);

        curr_grid.time_steps = readIntVar(tsFile);
        PCLOSE(tsFile);

        grids.push_back(curr_grid);
    }

    //Read eqGroup details
    char *eqGroupFile;
    STRINGIFY(eqGroupFile, "%s/eqGroupFile.txt",localDir);
    if(mpi_man->myRank == 0)
    {
        SYSTEM(sysLogFileName, "%s/getEqnGroups.sh %s/build_out.txt %s/build/stencil_code.hpp %s", TOOL_DIR, localDir, localDir, eqGroupFile);
    }
    mpi_man->global_barrier();
    std::vector<std::string> eqGroupName;
    std::vector<std::string> readGrids;
    std::vector<std::string> writeGrids;
    std::vector<int> n_reads;
    std::vector<int> n_writes;
    std::vector<int> n_stencils;
    readTable(eqGroupFile, eqGroupName, 0, -1, 0, 0);
    readTable(eqGroupFile, readGrids, 0, -1, 1, 1);
    readTable(eqGroupFile, writeGrids, 0, -1, 2, 2);
    readTable(eqGroupFile, n_reads, 0, -1, 3, 3);
    readTable(eqGroupFile, n_writes, 0, -1, 4, 4);
    readTable(eqGroupFile, n_stencils, 0, -1, 5, 5); // this one is wrong
    free(eqGroupFile);

    num_eqns=(int)eqGroupName.size();
    s = 0;
    maxNumStencils = 0;

    /*
    FILE *totStencilFile;
    POPEN(sysLogFileName, totStencilFile, "%s/getTotalStencils.sh %s/stencilCode.cpp", TOOL_DIR, localDir);
    int total_stencils=readIntVar(totStencilFile);
    bool baseStencil = false;
    if(total_stencils == -1)
    {
        baseStencil = true;
    }
    PCLOSE(totStencilFile);
    printf("totalStencils = %d.......\n", total_stencils);
    int rem_stencils= total_stencils;
    */
    for(int i=0; i<(int)eqGroupName.size(); ++i)
    {
        EQ_GROUP curr_eq_group;
        //std::cout<<eqGroupName[i]<<" "<<n_reads[i]<<" "<<n_writes[i]<<std::endl;
        eqGroupMap[eqGroupName[i]] = i;
        curr_eq_group.name=eqGroupName[i];
        curr_eq_group.num_reads=n_reads[i];
        curr_eq_group.num_writes=n_writes[i];
        //split grids in the string, with comma delimiter
        std::vector<std::string> readGridVec = split(readGrids[i],',');
        std::vector<std::string> writeGridVec = split(writeGrids[i],',');

        //int n_stencils = 0;
        //mapped indices of read grids
        for(int j=0; j<(int)readGridVec.size(); ++j)
        {
            int idx = gridMap[readGridVec[j]];
            curr_eq_group.read_grids.push_back(grids[idx]);

    /*        int halo_flag = 0;
            std::vector<int> curr_halos = grids[idx].halo;
            //if no halo it means there is no stencil operation
            for(int k=0; k<(int)curr_halos.size(); ++k)
            {
                if(curr_halos[k] != 0)
                {
                    halo_flag = 1;
                    break;
                }
            }

            if(halo_flag != 0)
            {
                n_stencils++;
            }

            printf("grid = %s, halo = %d\n", readGridVec[j].c_str(), halo_flag);
            */
        }

        //mapped indices of write grids
        for(int j=0; j<(int)writeGridVec.size(); ++j)
        {
            int idx = gridMap[writeGridVec[j]];
            curr_eq_group.write_grids.push_back(grids[idx]);
        }

        curr_eq_group.finalize();

        printf("reads = %d, writes = %d\n", curr_eq_group.num_spatial_reads, curr_eq_group.num_spatial_writes);
        //max is the value of s
        s=std::max(s, curr_eq_group.num_spatial_reads+curr_eq_group.num_spatial_writes);
        /*if(rem_stencils < 0)
        {
            ERROR_PRINT("Couldn't automatically assign stencil per equation group correctly\n");
        }*/
        curr_eq_group.num_stencils=n_stencils[i];
        if(curr_eq_group.num_stencils > curr_eq_group.num_reads)
        {
            ERROR_PRINT("Stencils greater than read grids; i.e., %d > %d", curr_eq_group.num_stencils, curr_eq_group.num_reads);
        }
        //std::min(rem_stencils,std::min(n_stencils,curr_eq_group.num_spatial_reads)); //It can fail if multiple eq. groups is there
        //printf("######## nstencils = %d, num_stencils = %d\n", n_stencils, curr_eq_group.num_stencils);
        //rem_stencils -= n_stencils;
        //printf("...-----...---Reads = %d, Writes = %d, Stencils = %d\n", curr_eq_group.num_spatial_reads, curr_eq_group.num_spatial_writes, curr_eq_group.num_stencils);

        maxNumStencils=std::max(maxNumStencils, curr_eq_group.num_stencils);
        eqGroups.push_back(curr_eq_group);
    }

    FILE *numMainEqnsFile;
    POPEN(sysLogFileName, numMainEqnsFile, "%s/getNumMainEqns.sh %s", TOOL_DIR, localDir);
    numMainEqns=readIntVar(numMainEqnsFile);
    PCLOSE(numMainEqnsFile);

    INFO_PRINT("stencil radius=%d, s=%d, numMainEqns=%d", radius, s, numMainEqns);

    //copy to stencilDetails all unfilled details
    stencilDetails->radius = radius;
    stencilDetails->s = s;
    stencilDetails->maxNumStencils = maxNumStencils;
    stencilDetails->folder = localDir;
    stencilDetails->data = this;

    models.clear();

/*    char *mc_file_loc;
    STRINGIFY(mc_file_loc, "%s/mc_file.txt", TOOL_DIR);

    FILE *file = fopen(mc_file_loc, "r");
    char *mc_file = readStrVar(file);
    fclose(file);
    free(mc_file_loc);
*/
    char *mc_file = glb_mc_file;

    FILE *threadPerSocketFile;
    POPEN(sysLogFileName, threadPerSocketFile, "%s/threadPerNUMA.sh %s", TOOL_DIR, mc_file);
    thread_per_socket = readIntVar(threadPerSocketFile);
    PCLOSE(threadPerSocketFile);


    //find cpu frequency
    FILE *cpuFreqFile;
    POPEN(sysLogFileName, cpuFreqFile, "%s/getFreq.sh %s", TOOL_DIR, mc_file);
    cpu_freq = readDoubleVar(cpuFreqFile);
    cpu_freq=cpu_freq; //Convert to GHz
    PCLOSE(cpuFreqFile);
   // free(mc_file);
    printf("CPU freq = %f GHz\n", cpu_freq);


    for(int i=0; i<numMainEqns; ++i)
    {
        char* iacaOut;
        STRINGIFY(iacaOut, "%s/iacaFolder_%d", localDir, i);
        char* CAP_arch = CAPITALIZE(arch);
        printf("%s\n", CAP_arch);

        if(strcmp(CAP_arch, "KNL") == 0)
        {
            free(CAP_arch);
            STRINGIFY(CAP_arch, "SKX");
        }

        //only one rank generates the file
        if(mpi_man->myRank==0)
        {
            SYSTEM(sysLogFileName,"%s/generateIACA.sh %d %s \"%s\" %s %s", TOOL_DIR, i+1, localDir, IACA_PATH, CAP_arch, iacaOut);
        }

        mpi_man->global_barrier();

        models.push_back(new perfModel(stencilDetails, cpu_freq, iacaOut));

        free(CAP_arch);
        delete[] iacaOut;
    }

    mpi_man->global_barrier();

    for(int i=0; i<mpi_man->nRanks; ++i)
    {
        if(i==mpi_man->myRank)
        {
#pragma omp parallel
            {
                PRINT_LOG("Thread %d : on rank %d set to cpu %d\n", omp_get_thread_num(), mpi_man->myRank, sched_getcpu());
            }
        }
        mpi_man->global_barrier();
    }
}

yaskSite::~yaskSite()
{
#ifndef CACHE_BUILD
    cleanDir();
#endif

    delete[] localDir;
    delete[] sysLogFileName;
    delete[] stencilCode;

    for(int i=0; i<numMainEqns; ++i)
    {
        delete models[i];
        models[i] = NULL;
    }

    //if there is an active kernel
    //delete it
    if(stencilFactory)
    {
        if(dynFinalize(this)<0)
        {
            ERROR_PRINT("Stencil is not generated");
        }
    }

    if(dl_handle)
    {
        //close loaded library
        dlclose(dl_handle);
        dl_handle = NULL;
    }

    if(STENCIL_struct_alloc)
    {
        delete stencilDetails;
    }
}

void yaskSite::cleanDir()
{
    //remove the localDir
    SYSTEM_WO_PIPE(sysLogFileName, "rm -r -f %s", localDir);
}


//added -fPIC for compiling as shared lib;
/*#define BUILD_CMD\
    "make -C %s EXTRA_CXXFLAGS=-fPIC stencil=%s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=8 SUB_BLOCK_LOOP_INNER_MODS=\"%s\" SUB_BLOCK_LOOP_OUTER_MODS=\"%s\" likwid=%d EXTRA_LOOP_OPTS=\"-innerMod '_Pragma(\\\"ivdep\\\") _Pragma(\\\"unroll(4)\\\")'\"", yaskDir, stencil, arch, fold_x, fold_y, fold_z, (prefetch)?"prefetch(L2)":"", path, (int) buildWithLikwid\
*/


#define BUILD_CMD\
    "mkdir -p %s/asm && make -C %s EXTRA_CXXFLAGS=\"-fPIC -D__PURE_INTEL_C99_HEADERS__\" %s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=%d REGION_LOOP_OPTS=\"-ndims 4 -inVar region_idxs -ompConstruct 'omp parallel for schedule(static,1) proc_bind(close)'\" pfd_l1=%d pfd_l2=%d MINI_BLOCK_LOOP_OPTS=\"-ndims 4 -inVar adj_mb_idxs -ompConstruct ''\" layout_txyz=Layout_1234 layout_twxyz=Layout_12345 halo=%d YASK_OUT_BASE=%s YC_STENCIL_DIR=%s/stencils EXTRA_YC_FLAGS=\"-print-eqs\" EXTRA_YK_CXXFLAGS=\"-Fa%s/asm/\" > %s/build_out.txt", localDir, yaskDir, stencil_with_radius, arch, fold_x, fold_y, fold_z, (dp)?8:4, (prefetch)?1:0, (prefetch)?1:0, radius, localDir, localDir, localDir, localDir\


/*
#define BUILD_CMD\
    "make -C %s EXTRA_CXXFLAGS=\"-fPIC -D__PURE_INTEL_C99_HEADERS__\" %s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=%d SUB_BLOCK_LOOP_INNER_MODS=\"%s\" SUB_BLOCK_LOOP_OUTER_MODS=\"%s\" BLOCK_LOOP_OPTS=\"-dims 'bw,bx,by,bz' -ompConstruct ''\" layout_txyz=Layout_1234 layout_twxyz=Layout_12345 likwid=%d halo=%d", yaskDir, stencil_with_radius, arch, fold_x, fold_y, fold_z, (dp)?8:4, (prefetch)?"prefetch(L2)":"", path, (int) buildWithLikwid, radius\
*/

/*
#define BUILD_CMD\
    "make -C %s EXTRA_CXXFLAGS=\"-fPIC -D__PURE_INTEL_C99_HEADERS__\" %s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=8 SUB_BLOCK_LOOP_INNER_MODS=\"%s\" SUB_BLOCK_LOOP_OUTER_MODS=\"%s\" layout_txyz=Layout_1234 likwid=%d halo=%d", yaskDir, stencil_with_radius, arch, fold_x, fold_y, fold_z, (prefetch)?"prefetch(L2)":"", path, (int) buildWithLikwid, radius\
*/
void yaskSite::build()
{
    SYNC_WITH_DIM(fold_z, fold_y, fold_x, "fold");

    char *lib;
    STRINGIFY(lib, "%s/lib/libYASK.so", localDir);

    //check if library already exists, if yes don't rebuild
    void* test_open = dlopen(lib, RTLD_LOCAL|RTLD_NOW);
    if(test_open == NULL || cacheStale)
    {
        //close the opened library so I can edit it, if cache is stale
        if(test_open && cacheStale)
        {
            dlclose(test_open);
        }

       //Need to newly build
        //clenaing source build before doing anything
        cleanBuild();

        char* stencil_with_radius;
        if(radius==-1)
        {
            STRINGIFY(stencil_with_radius, "stencil=%s", stencil);
        }
        else
        {
            STRINGIFY(stencil_with_radius, "stencil=%s radius=%d", stencil, radius);
        }

        //only one rank generates the stencil
        if(mpi_man->myRank == 0)
        {
            PRINT_LOG("BUILD CMD = \n");
            PRINT_LOG(BUILD_CMD);
            printf("\n");
            LOAD_PRINT_START("Building %s : %s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=%d prefetch=%s layout_txyz=Layout_1234 path=%s", stencilCode, stencil_with_radius, arch, fold_x, fold_y, fold_z, (dp)?8:4, (prefetch)?"on":"off", (strcmp(path,"")==0)?"default":path);
            SYSTEM(sysLogFileName, BUILD_CMD);
            mpi_man->global_barrier();
            delete[] stencil_with_radius;

            //SYSTEM(sysLogFileName, "cp %s/bin/yask.%s.%s.exe %s/bin/.", yaskDir, stencil, arch, localDir);

            SYSTEM(sysLogFileName, "%s/createYASKLib.sh %s %s %s %s %s %s", TOOL_DIR, yaskDir, stencil, arch, SRC_DIR, INC_DIR, localDir);
            LOAD_PRINT_END();
        }

        //dlopen and link them
        //generated library
        LOAD_PRINT_START("Loading %s", stencilCode);

        free(lib);
        STRINGIFY(lib, "%s/lib/libYASK.so", localDir);
        dl_handle = dlopen(lib, RTLD_LOCAL|RTLD_NOW);
        if(!dl_handle)
        {
            ERROR_PRINT("Could not open generated library. Error: %s\n", dlerror());
        }

        LOAD_PRINT_END();
    }
    else
    {
        INFO_PRINT("Stencil %s : found in cache", stencilCode);
        dl_handle = test_open;
    }

    //reset error
    dlerror();
    dynInit = (fn0_t) dlsym(dl_handle, "YASKinit");

    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKinit");
    }
    //reset error
    dlerror();
    dynFinalize = (fn1_t) dlsym(dl_handle, "YASKfinalize");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKfinalize");
    }
    //reset error
    dlerror();
    dynStencil = (fn2_t) dlsym(dl_handle, "YASKstencil");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKstencil");
    }

    //reset error
    dlerror();
    dynGetElement = (fn3_t) dlsym(dl_handle, "YASKgetElement");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKgetElement");
    }

    //reset error
    dlerror();
    dynSetElement = (fn4_t) dlsym(dl_handle, "YASKsetElement");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKsetElement");
    }

    //reset error
    dlerror();
    dynSetAll = (fn5_t) dlsym(dl_handle, "YASKsetAll");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKsetAll");
    }

    dlerror();
    dynGetGridPtr = (fn6_t) dlsym(dl_handle, "YASKgetGridPtr");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKgetGridPtr");
    }

    dlerror();
    dynFuseGrid = (fn7_t) dlsym(dl_handle, "YASKfuseGrid");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKfuseGrid");
    }

    dlerror();
    dynGetHalo = (fn8_t) dlsym(dl_handle, "YASKgetHalo");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKgetHalo");
    }

    dlerror();
    dynYASK_AT = (fn9_t) dlsym(dl_handle, "YASKautoTuner");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKautoTuner");
    }


    /*
    dlerror();
    dynSetHalo = (fn7_t) dlsym(dl_handle, "YASKsetHalo");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKsetHalo");
    }*/


    delete[] lib;
    stencilReady=true;
}

void yaskSite::cleanBuild()
{
    SYSTEM_WO_PIPE(sysLogFileName, "make -C %s clean", yaskDir);
    SYSTEM_WO_PIPE(sysLogFileName, "rm -f %s/cxx-flags.%s.%s.txt %s/ld-flags.%s.%s.txt %s/make-report.%s.%s.txt", yaskDir, stencil, arch, yaskDir, stencil, arch, yaskDir, stencil, arch);
}

void yaskSite::setSubBlock(int z, int y, int x)
{
    SYNC_WITH_DIM(z, y, x, "subBlock");
    sbx=x;
    sby=y;
    sbz=z;
    subBlockUpdated = true;
}


void yaskSite::setS(int s_val)
{
    s=s_val;
}

void yaskSite::setBlock(int z, int y, int x, bool resetOthers)
{
    SYNC_WITH_DIM(z, y, x, "block");
    bx=x;
    by=y;
    bz=z;
    blockUpdated = true;

    if(resetOthers)
    {
        setDefaultSubBlock();
    }
}

void yaskSite::setRegion(int z, int y, int x, int t, bool resetOthers)
{
    SYNC_WITH_DIM(z, y, x, "region");
    rt=t;
    rx=x;
    ry=y;
    rz=z;
    regionUpdated = true;

    if(resetOthers)
    {
        setDefaultBlock();
        setDefaultSubBlock();
    }
}

void yaskSite::setDim(int z, int y, int x, int t, bool resetOthers)
{
    SYNC_WITH_DIM(z, y, x, "dim");
    dt=t;
    dx=x;
    dy=y;
    dz=z;

    /*Updtaing padded dimensions*/
    int pad_x=(dx%fold_x)?(fold_x-(dx%fold_x)):0;
    dx_p = dx + pad_x;

    int pad_y=(dy%fold_y)?(fold_y-(dy%fold_y)):0;
    dy_p = dy + pad_y;

    int pad_z=(dz%fold_z)?(fold_z-(dz%fold_z)):0;
    dz_p = dz + pad_z;

    dx=dx_p;
    dy=dy_p;
    dz=dz_p;

    if(resetOthers)
    {
        setDefaultRegion();
        setDefaultBlock();
        setDefaultSubBlock();
    }

    dimUpdated = true;

}

void yaskSite::setThread(int nthreads_, int threadPerBlock_, bool resetOthers)
{
    nthreads = nthreads_;
    if(nthreads > thread_per_socket)
    {
        //nthreads = thread_per_socket;
        WARNING_PRINT("Currently the framework only supports performance modeling for 1 ccNUMA domain\n");
    }
    threadPerBlock = threadPerBlock_;
    if(resetOthers)
    {
        //again reset default
        setDefaultBlock();
        setDefaultSubBlock();
    }

    threadUpdated = true;
}

void yaskSite::setDefaultRegion()
{
    regionUpdated = true;

    rt = 0; //no temporal //dt;
    if(dim==1)
    {
        rx = 1;
        ry = 1;
        rz = dz;
    }
    else if(dim==2)
    {
        rx = 1;
        ry = dy;
        rz = dz;
    }
    else
    {
        rx = dx;
        ry = dy;
        rz = dz;
    }

    /*Updtaing padded dimensions*/
    int pad_x=(rx%fold_x)?(fold_x-(rx%fold_x)):0;
    rx_p = rx + pad_x;

    int pad_y=(ry%fold_y)?(fold_y-(ry%fold_y)):0;
    ry_p = ry + pad_y;

    int pad_z=(rz%fold_z)?(fold_z-(rz%fold_z)):0;
    rz_p = rz + pad_z;
}


void yaskSite::setDefaultBlock()
{
    blockUpdated = true;
    if(dim==1)
    {
        bx = 1;
        by = 1;
        bz = static_cast<int>(round(threadPerBlock*rz_p/((double)nthreads)));
    }
    else if(dim==2)
    {
        bx = 1;
        //Similar to OMP on outer-most loop
        //TODO inner shouldn't in priciple hurt in yask if loop size is big
        //enough
        double test_by = threadPerBlock*ry_p/((double)nthreads);
        if(test_by >= 1)
        {
            by = static_cast<int>(round(threadPerBlock*ry_p/((double)nthreads)));
            bz = rz_p;
        }
        else
        {
            by = ry_p;
            bz = static_cast<int>(round(threadPerBlock*rz_p/((double)nthreads)));
        }
    }
    else if(dim==3)
    {
        //Loop to parallelise can be selected by benchmarks, may be hybrid is
        //good in this case but I wouldn't do this here,
        //To get good values there are 2 options:
        //1. use loopTuner
        //2. manually set Block and subBlock
        //default outer loop will be parallelised
        double test_bx = threadPerBlock*rx_p/((double)nthreads);
        double test_by = threadPerBlock*ry_p/((double)nthreads);
        if(test_bx >= 1)
        {
            bx = static_cast<int>(round(threadPerBlock*rx_p/((double)nthreads)));
            by = ry_p;
            bz = rz_p;
        }
        else if(test_by >= 1)
        {
            bx = rx_p;
            by = static_cast<int>(round(threadPerBlock*ry_p/((double)nthreads)));
            bz = rz_p;
        }
        else
        {
            bx = rx_p;
            by = ry_p;
            bz = static_cast<int>(round(threadPerBlock*rz_p/((double)nthreads)));
        }
    }

    PRINT_LOG("In default : Current block sizes (bx, by, bz) = (%d, %d, %d)\n", bx, by, bz);
}


int makeGoodBigNumber(int bigNum, int smallNum, int totCtr, int inc)
{

    if(smallNum<1)
    {
        smallNum=1;
    }
    int ctr = 0;
    double minRem = bigNum%smallNum;
    int min_bigNum = bigNum;

    if((inc < 0) && (bigNum < smallNum))
    {
        min_bigNum = smallNum;
    }
    else
    {
        //make n_scale_down_olc good multiple of ry
        while((bigNum%smallNum != 0)&&(ctr<totCtr))
        {
            bigNum=bigNum+inc;
            ++ctr;
            double currRem = static_cast<double>(bigNum%smallNum);

            //printf("smallNum = %d, bigNum = %d, currRem = %f\n", smallNum, bigNum, currRem);
            if(currRem < minRem)
            {
                min_bigNum = bigNum;
                minRem = currRem;
            }
        }
    }

    return min_bigNum;
}


int makeGoodNumber(int bigNum, int smallNum, int totCtr)
{

    int ctr = 0;
    double minRem = bigNum%smallNum;
    int min_smallNum = smallNum;

    //make n_scale_down_olc good multiple of ry
    while((bigNum%smallNum != 0)&&(ctr<totCtr))
    {
        ++smallNum;
        ++ctr;
        double currRem = static_cast<double>(bigNum%smallNum);

        printf("smallNum = %d, bigNum = %d, currRem = %f\n", smallNum, bigNum, currRem);
        if(currRem < minRem)
        {
            min_smallNum = smallNum;
            minRem = currRem;
        }
    }

    return min_smallNum;
}

//Returns threads in x,y, and z dimension
std::vector<int> yaskSite::setDefaultBlock_min_rem(int n_scale_down_olc, int n_scale_down_ilc, char* temporal_str)
{
    blockUpdated = true;
    bool found = false;
    //First element flag for checking if blocking carried out, the rest three
    //are thread count in each 3 dimensions
    std::vector<int> retThreads(4,1);
    retThreads[0] = 0;

    if(dim==1)
    {
        bx = 1;
        by = 1;
        bz = static_cast<int>(round(threadPerBlock*rz_p/((double)nthreads)));
        found = true;
        retThreads[3] = nthreads;
    }
    else if(dim==2)
    {
        bx = 1;
        by_min = std::min(ry, (16 + 4*static_cast<int>(fold_y/4.0)*fold_y));
        bz_min = std::min(rz, 128 + 0*static_cast<int>(fold_z/4.0)*fold_z);
        if(ry < nthreads) //it can't cut if this is the case
        {
            bz_min = fold_z;
        }

        std::vector<double> reminder_size;
        std::vector<int> select_by;
        std::vector<int> select_bz;
        std::vector<int> select_ty;
        std::vector<int> select_tz;


        int curr_threads = nthreads;
        //Similar to OMP on outer-most loop
        //TODO inner shouldn't in priciple hurt in yask if loop size is big
        //enough
        while(!found)
        {
            //find good multiple of curr_threads
            std::vector<int> outer_threads;
            std::vector<int> inner_threads;

            for(int i=curr_threads; i>=1; --i)
            {
                /*In the order outer has more threads first*/
                int out=i;
                int in=curr_threads/i;

                if(out*in == curr_threads)
                {
                    outer_threads.push_back(out);
                    inner_threads.push_back(in);
                }
            }

            int curr_by, curr_bz;
            bool min_set = false;
            while(!found)
            {
                for(int i=0; i<(int)outer_threads.size(); ++i)
                {
                    int out = outer_threads[i];
                    int in  = inner_threads[i]*n_scale_down_olc;

                    curr_by = static_cast<int>(round(threadPerBlock*ry_p/((double)out)));
                    curr_bz = static_cast<int>(round(threadPerBlock*rz_p/((double)in)));

                    if((curr_by >= by_min) && (curr_bz >= bz_min) )
                    {
                        //will have to rank based on reminder; this makes to select the
                        //block with good load balancing
                        reminder_size.push_back( ((curr_by*out)-ry_p)*rz_p + (curr_bz*in-rz_p) );
                        select_by.push_back(curr_by);
                        select_bz.push_back(curr_bz);
                        select_ty.push_back(out);
                        select_tz.push_back(in);
                        found = true;
                        //select this break
                        //break;
                    }
                }
                if(!found && !min_set)
                {
                    by_min = 2;
                    bz_min = 2;
                    min_set=true;
                }
                else
                {
                    break;
                }
            }

            --curr_threads; //extreme measure reduce curr_threads
        }

        std::vector<int> perm(reminder_size.size());
        for(int i=0; i<(int)reminder_size.size();++i)
        {
            perm[i]=i;
        }

        //select the one with lowest reminder
        std::stable_sort(perm.begin(), perm.end(), [&] (const int &a, const int &b) { return (reminder_size[a] < reminder_size[b]); } );


        //The first one should have lowest reminder
        by=select_by[perm[0]];
        bz=select_bz[perm[0]];
        retThreads[2] = select_ty[perm[0]];
        retThreads[3] = select_tz[perm[0]];
   }
    else if(dim==3)
    {
        //Loop to parallelise can be selected by benchmarks, may be hybrid is
        //good in this case but I wouldn't do this here,
        //To get good values there are 2 options:
        //1. use loopTuner
        //2. manually set Block and subBlock
        //default outer loop will be parallelised

        //minimums depends on folds; check with benchmark good folds
        double radius_x, radius_y, radius_z;
        getRadiusFolded(radius_x, this, x);
        getRadiusFolded(radius_y, this, y);
        getRadiusFolded(radius_z, this, z);


        //int bx_min = std::min(rx, 16 + static_cast<int>(fold_x/4.0)*fold_x);
        //int by_min = std::min(ry, 8 + 2*static_cast<int>(fold_y/4.0)*fold_y);
        bx_min = std::min(rx, std::max(5,static_cast<int>(3*radius_x)));
        by_min = std::min(ry, std::max(5,static_cast<int>(3*radius_y)));
        bz_min = std::min(rz, 64 + 8*static_cast<int>(fold_z/4.0)*fold_z);

        if(rx*ry < nthreads)
        {
            bz_min = fold_z;
        }

        printf("min %dx%dx%d\n",bx_min,by_min,bz_min);

        //find good multiple of nthreads
        std::vector<int> outer_threads;
        std::vector<int> middle_threads;
        std::vector<int> inner_threads;

        std::vector<double> reminder_size;
        std::vector<int> select_bx;
        std::vector<int> select_by;
        std::vector<int> select_bz;

        std::vector<int> select_tx;
        std::vector<int> select_ty;
        std::vector<int> select_tz;

        bool min_set = false;
        int cur_threads = nthreads;
        int min_ctr = 0;
        //make n_scale_down_olc good multiple of ry
        n_scale_down_olc = makeGoodNumber(ry_p, n_scale_down_olc, 20);
        //make good multiple of nthreads
        if(n_scale_down_olc > 0.5*nthreads)
        {
            n_scale_down_olc = makeGoodBigNumber(n_scale_down_olc, nthreads, nthreads);
        }
        while(!found)
        {
            reminder_size.clear();
            outer_threads.clear();
            middle_threads.clear();
            inner_threads.clear();

            for(int i=cur_threads; i>=1; --i)
            {
                int out=i;
                int rest=cur_threads/i;

                if(out*rest == cur_threads)
                {
                    for(int j=rest; j>=1; --j)
                    {
                        int middle=j;
                        int in = rest/j;

                        if(middle*in == rest)
                        {
                            outer_threads.push_back(out);
                            middle_threads.push_back(middle);
                            inner_threads.push_back(in);
                        }
                    }
                }
            }

            //find good multiple of scale factor
            std::vector<int> outer_multiple;
            std::vector<int> inner_multiple;

            //make n_scale_down_olc good multiple of ry
            //n_scale_down_olc = makeGoodNumber(ry_p, n_scale_down_olc, 20);

            printf("threads = %d, olc_scale = %d, ilc_scale = %d\n", cur_threads, n_scale_down_olc, n_scale_down_ilc);
            printf("min_set = %d, bx=%d, by=%d, bz=%d\n", min_set, bx_min, by_min, bz_min);
            for(int i=1; i<=n_scale_down_olc; ++i)
            {
                /*In the order outer has more threads first*/
                int in=i;
                int out=ceil(n_scale_down_olc/static_cast<double>(i));

                //if(out*in == (n_scale_down_olc))
                {
                    outer_multiple.push_back(out);
                    inner_multiple.push_back(in);
                }
            }

            int curr_bx, curr_by, curr_bz;

            for(int i=0; i<(int)outer_threads.size(); ++i)
            {
                for(int j=0; j<(int)outer_multiple.size(); ++j)
                {
                    int out = outer_threads[i];
                    int middle = middle_threads[i];
                    int in = inner_threads[i];

                    if(middle < outer_multiple[j])
                    {
                        middle = static_cast<int>(outer_multiple[j]/middle_threads[i])*middle_threads[i];
                        if(middle < outer_multiple[j])
                        {
                            middle = static_cast<int>(outer_multiple[j]/middle_threads[i]+1)*middle_threads[i];
                        }
                    }

                    if(in < inner_multiple[j])
                    {
                        in = static_cast<int>(inner_multiple[j]/inner_threads[i])*inner_threads[i];
                        if(in < inner_multiple[j])
                        {
                            in = static_cast<int>(inner_multiple[j]/inner_threads[i]+1)*inner_threads[i];
                        }
                    }

                    if(in < n_scale_down_ilc)
                    {
                        in = static_cast<int>(n_scale_down_ilc/inner_threads[i])*inner_threads[i];
                        if(in < n_scale_down_ilc)
                        {
                            in = static_cast<int>(n_scale_down_ilc/inner_threads[i]+1)*inner_threads[i];
                        }
                    }

                    curr_bx = static_cast<int>(round(threadPerBlock*rx_p/((double)out)));
                    curr_by = static_cast<int>(round(threadPerBlock*ry_p/((double)middle)));
                    curr_bz = static_cast<int>(round(threadPerBlock*rz_p/((double)in)));


                    //make them multiple of radius else cut in between
                    //The block reminder hurts more than fold reminder
                    /*curr_bx = makeGoodBigNumber(curr_bx, (int)radius_x, (int)radius_x, -1);
                    curr_by = makeGoodBigNumber(curr_by, (int)radius_y, (int)radius_y, -1);
                    curr_bz = makeGoodBigNumber(curr_bz, (int)radius_z, (int)radius_z, -1);
                    */
                    if( (curr_bx >= bx_min) && (curr_by >= by_min) && (curr_bz >= bz_min) )
                    {
                        printf("out = %d, middle = %d, in =  %d\n", out, middle, in);
                        printf("out_b = %d, middle_b = %d, in_b =  %d\n", curr_bx, curr_by, curr_bz);
                        //will have to rank based on reminder; this makes to select the
                        //block with good load balancing
                        //reminder_size.push_back(  std::abs(((curr_bx*out)-rx_p)*ry_p*rz_p + ((curr_by*middle)-ry_p)*rz_p + (curr_bz*in-rz_p)) );
                        //latency added to z dimension to make sure it is
                        //hurtful
                        cache_info temporalCache = CACHE(temporal_str);
                        double lat = temporalCache.getLatency(1,cur_threads);
                        double cy_lup = 5; //just an assumption
                        //2.2 freq., assumption
                        double lup_per_lat = lat*2.2/cy_lup;
                        double lat_penalty = 0;
                        if(in>1)
                        {
                            lat_penalty = lup_per_lat/curr_bz;
                        }
                        reminder_size.push_back(  std::abs(((curr_bx*out)-rx_p)/rx_p + ((curr_by*middle)-ry_p)/ry_p + (curr_bz*in-rz_p)/rz_p + lat_penalty) );
                        printf("lat_penalty = %f, reminder = %f\n", lat_penalty, reminder_size.back());
                        select_bx.push_back(curr_bx);
                        select_by.push_back(curr_by);
                        select_bz.push_back(curr_bz);

                        select_tx.push_back(out);
                        select_ty.push_back(middle);
                        select_tz.push_back(in);

                        //select this
                        found = true;
                        //break;
                    }
                }
            }

            if(!found)
            {
                if(!min_set)
                {

                    if(min_ctr < 1)
                    {
                        bx_min = std::min(rx, static_cast<int>(3*radius_x)); //can set to radius
                        by_min = std::min(ry, static_cast<int>(3*radius_y)); //can set to radius
                        bz_min = std::min(rz, static_cast<int>(16*radius_z));
                        min_ctr++;
                    }
                    else if(min_ctr == 1)
                    {
                        bx_min = std::min(rx, static_cast<int>(2*radius_x)); //can set to radius
                        by_min = std::min(ry, static_cast<int>(2*radius_y)); //can set to radius
                        bz_min = std::min(rz, static_cast<int>(16*radius_z));
                        min_ctr++;
                    }
                    else if(min_ctr == 2)
                    {
                        bx_min = std::min(rx, static_cast<int>(3*radius_x)); //can set to radius
                        by_min = std::min(ry, static_cast<int>(3*radius_y)); //can set to radius
                        bz_min = std::min(rz, static_cast<int>(8*radius_z));
                        min_ctr++;
                    }
                    else if(min_ctr == 3)
                    {
                        bx_min = std::min(rx, static_cast<int>(2*radius_x)); //can set to radius
                        by_min = std::min(ry, static_cast<int>(2*radius_y)); //can set to radius
                        bz_min = std::min(rz, static_cast<int>(8*radius_z));
                        min_ctr++;
                    }
                    else if(min_ctr == 4)
                    {
                        bx_min = std::min(rx, static_cast<int>(radius_x)); //can set to radius
                        by_min = std::min(ry, static_cast<int>(radius_y)); //can set to radius
                        bz_min = std::min(rz, static_cast<int>(16*radius_z));
                        min_ctr++;
                    }
                    else if(min_ctr == 5)
                    {
                        bx_min = std::min(rx, static_cast<int>(radius_x)); //can set to radius
                        by_min = std::min(ry, static_cast<int>(radius_y)); //can set to radius
                        bz_min = std::min(rz, static_cast<int>(8*radius_z));
                        min_ctr++;
                    }
                    else
                    {
                        bx_min = std::max(1, fold_x);
                        by_min = std::max(1, fold_y);
                        bz_min = std::max(4, fold_z);
                        min_set = true;
                    }
                }
                else
                {
                    cur_threads--;
                }
            }
            //cur_threads--;
        }

        std::vector<int> perm(reminder_size.size());
        for(int i=0; i<(int)reminder_size.size();++i)
        {
            perm[i]=i;
        }

        //select the one with lowest reminder
        std::stable_sort(perm.begin(), perm.end(), [&] (const int &a, const int &b) { return (reminder_size[a] < reminder_size[b]); } );

        int lowest_rem = (int)reminder_size[perm[0]];
        int lowest_threads = select_tx[perm[0]]*select_ty[perm[0]]*select_tz[perm[0]];
        int highest_bz = select_bz[perm[0]];
        int idx=0;
        int select_idx=0;
        while((idx<(int)reminder_size.size()) && ((int)reminder_size[perm[idx]]==lowest_rem))
        {
            int curr_threads = select_tx[perm[idx]]*select_ty[perm[idx]]*select_tz[perm[idx]];
            int curr_bz = select_bz[perm[idx]];

            printf("idx = %d, size = %d, curr_bz = %d, highest_bz = %d\n", idx, (int)reminder_size.size(), curr_bz, highest_bz);
            if((curr_threads < lowest_threads) && (curr_bz >= highest_bz))
            {
                select_idx = idx;
                lowest_threads = curr_threads;
                highest_bz = curr_bz;
            }
            idx++;
        }

        printf("settled \n");
        bx = select_bx[perm[select_idx]];
        by = select_by[perm[select_idx]];
        bz = select_bz[perm[select_idx]];

        retThreads[1] = select_tx[perm[select_idx]];
        retThreads[2] = select_ty[perm[select_idx]];
        retThreads[3] = select_tz[perm[select_idx]];

        if(!found)
        {
            WARNING_PRINT("Ideal parameter not found- performance degradation to be expected");
        }
    }
    retThreads[0] = found;

    return retThreads;
}

void yaskSite::setDefaultSubBlock()
{
    subBlockUpdated = true;
    if(dim==1)
    {
        sbx = 1;
        sby = 1;
        sbz = static_cast<int>(round(bz/((double)threadPerBlock)));
    }
    else if(dim==2)
    {
        //Similar to OMP on outer-most loop
        //TODO inner shouldn't in priciple hurt in yask if loop size is big
        //enough;
        sbx = 1;
        sby = static_cast<int>(round(by/((double)threadPerBlock)));
        sbz = bz;
    }
    else if(dim==3)
    {
        //Loop to parallelise can be selected by benchmarks, may be hybrid is
        //good in this case but I wouldn't do this here,
        //To get good values there are 2 options:
        //1. use loopTuner(TODO)
        //2. manually set Block and subBlock
        //default outer loop will be parallelised
        sbx = static_cast<int>(round(bx/((double)threadPerBlock)));
        sby = by;
        sbz = bz;
    }
}

/* Here we solve for the following optimization problem
 * obj: max(bz) and max(n)
 * st : 8*(2*r+1)*by*bz < OBC
 *      8*(2*r+1)*bz    < IBC
 *      n               = (dy*dz)/(by*bz)
 *      by              > by_min
 *      bz              > bz_min
 */

//converts by -> bz or bz ->by
#define f(b_)\
    (ry_p*rz_p)/(n*((double)b_))\

std::vector<int> yaskSite::spatialTuner(char* OBC_str, char* IBC_str, double sf_OBC_inp, double sf_IBC_inp, char* temporal_str)
{
    bool blocked = false;
    std::vector<int> retThreads(4,1);

    if(dim==1)
    {
        blocked = true;
    }

    else if(dim==2)
    {
        bx=1;
        by=ry_p;

        double layer;// = (2*radius+1+(s-1)); //layer width; assuming only one stencil in every eqn and the eqn. has a linear combination of others.
                                        // TODO: specifically tune.

        getMaxLayers(layer, stencilDetails, OUTER);
        cache_info OBC = CACHE(OBC_str);
        double sf_OBC = (sf_OBC_inp < 0) ? OBC.sf : sf_OBC_inp;
        double OBC_words = (OBC.shared)?(OBC.getWords()*sf_OBC)/OBC.cores:(OBC.getWords()*sf_OBC);

        double n_z = (layer*((double)rz_p))/(OBC_words);
        //round n_z to next greatest multiple of nthreads
        double n = ceil(n_z/nthreads)*nthreads;

        if(n_z>1)
        {
            bz = static_cast<int>(round(rz_p/n));
            retThreads[3] = nthreads;
            setDefaultSubBlock();
            INFO_PRINT("Found: bx=%d by=%d, bz=%d", bx, by, bz);
        }
        else
        {
            //setDefaultBlock();
            retThreads = setDefaultBlock_min_rem(1,1,temporal_str);
            setDefaultSubBlock();

            INFO_PRINT("Found: bx=%d by=%d, bz=%d", bx, by, bz);
            INFO_PRINT("Spatial Blocking not carried out, dim fits in cache");
        }

        blocked = true;
    }
    else
    {
        //don't make bz very small since its the inner dimension
        by_min = std::min(ry_p, 4 + 2*static_cast<int>(fold_y/8.0)*fold_y);
        bz_min = std::min(rz_p, 64 + 16*static_cast<int>(fold_z/8.0)*fold_z);

        printf("spatial min %dx%dx%d\n",0,by_min,bz_min);

        double layer_outer;// = (2*radius+1+(s-1)); //layer width
        getMaxLayers(layer_outer, stencilDetails, OUTER);

        printf("outer_layer = %f\n", layer_outer);
        double layer_inner;
        getMaxLayers(layer_inner, stencilDetails, INNER);

        cache_info OBC = CACHE(OBC_str);
        cache_info IBC = CACHE(IBC_str);

        double sf_OBC = (sf_OBC_inp < 0) ? OBC.sf : sf_OBC_inp;
        double sf_IBC = (sf_IBC_inp < 0) ? IBC.sf : sf_IBC_inp;

        printf("check OBC words = %f, sf = %f, shared = %s\n", OBC.getWords(), sf_OBC, OBC.shared?"shared":"not shared");
        //TODO: determine how many threads are currently sharing the resources
        //if its in shared mode
        double OBC_words = (OBC.shared)?(OBC.getWords()*sf_OBC)/OBC.cores:(OBC.getWords()*sf_OBC);
        double IBC_words = (IBC.shared)?(IBC.getWords()*sf_IBC)/IBC.cores:(IBC.getWords()*sf_IBC);


#ifdef MODEL_PREFETCH_EFFECT
        //adding prefetching effects
        OBC_words -= (OBC.prefetch_cl*8.0*layer_outer);
        IBC_words -= (IBC.prefetch_cl*8.0*layer_inner);
#endif

        printf("OBC = %f, ry = %d, rz = %d\n", OBC_words, ry_p, rz_p);
        double n_z = (layer_outer*((double)ry_p)*((double)rz_p))/(OBC_words);
        //round n_z to next greatest multiple of nthreads
        //double n = ceil(n_z/nthreads)*nthreads;
        //prefetching effect added
        double bz_start = std::min( (IBC_words/layer_inner), (double)(rz_p));
        //double bz_end = bz_min;
        //
        printf("check bz_start = %f\n", bz_start);
        //find bz_start: such that its perfect multiples of rz (to avoid reminder) and fold_z
        int bz_ratio_start = static_cast<int>(ceil(rz_p/bz_start));
        //int bz_ratio_end   = static_cast<int>(floor(rz_p/bz_end));

        //double bz_tune, by_tune;

        if(n_z>1)
        {
            printf("n_z = %f, start = %d\n", n_z, bz_ratio_start);
            retThreads = setDefaultBlock_min_rem(static_cast<int>(ceil(n_z)), static_cast<int>(bz_ratio_start), temporal_str);
            if(retThreads[0] == 1)
            {
                blocked = true;
            }
            setDefaultSubBlock();
            /*
            for(int bz_ratio = bz_ratio_start; bz_ratio <= bz_ratio_end; ++bz_ratio)
            {
                //now get correct bz_start and bz_end; with perfect multiples of rz and fold size
                bz_tune = rz_p/(static_cast<double>(bz_ratio));
                by_tune = f(bz_tune);

                if(by_tune >= by_min)
                {
                    blocked = true;
                    break;
                }
            }*/

            if(!blocked)
            {
                INFO_PRINT("Spatial Blocking not carried out, size smaller than minimums");
                //setDefaultBlock();
                retThreads = setDefaultBlock_min_rem(1,1,temporal_str);
                setDefaultSubBlock();
            }
            else
            {
                bx = static_cast<int>(round(bx));
                bz = static_cast<int>(round(bz));
                by = static_cast<int>(round(by));
                //now set subBlocks, based on by and bz
                setDefaultSubBlock();

                INFO_PRINT("Found: bx=%d by=%d, bz=%d", bx, by, bz);
            }
        }
        //check again if LC is satisfied else
        else
        {
            INFO_PRINT("Spatial Blocking not carried out, dim fits in cache");
            //setDefaultBlock();
            retThreads = setDefaultBlock_min_rem(1,1,temporal_str);
            setDefaultSubBlock();
            blocked = true;
        }
    }

    printf("Current block sizes (bx, by, bz) = (%d, %d, %d)\n", bx, by, bz);
    retThreads[0]=blocked;
    return retThreads;
}

bool yaskSite::temporalTuner(char  *cacheStr, double sf_inp)
{
    if(dt == 1)
    {
        WARNING_PRINT("Temporal blocking might degrade performance, since temporal dimension is 1");
    }

    int rx_min = std::min(dx,20);
    int min_y = 16;
    if(nthreads == 3 || nthreads ==5)
    {
        min_y = 15;
    }
    else if(nthreads == 7)
    {
        min_y = 14;
    }
    else if(nthreads == 9)
    {
        min_y = 9;
    }
    else if( (nthreads != 1) && ((nthreads%2) != 0) )
    {
        min_y = 10; //put it as a low value so it can cut in other dimensions if required
    }


    int ry_min = std::min(dy,min_y);
    int rz_min = std::min(dz,128);

        /*
    if(dim<3)
    {
        ry_min = 128;
        rz_min = 512;
    }
*/
    cache_info blockCache = CACHE(cacheStr);

    double sf = (sf_inp < 0)?blockCache.sf:sf_inp;

    if(!blockCache.shared)
    {
        WARNING_PRINT("Will not do any tuning for temporal blocking; because %s is not a shared cache", cacheStr);
    }

    //TODO: determine how many threads share the cache
    //maybe use hwloc
    double cacheWords = blockCache.getWords()*sf;

    double ryInit = cacheWords/((double)(s*dx_p*dz_p));
    //now set ry : ry is a proper multiple of dy; used ceil to absorb
    //if possible all the required decomposition here
    int factor_y = static_cast<int>(ceil(dy_p/ryInit));

    //reminders at regions might affect performance; since blocks at reminder
    //region cannot be distributed to threads properly
    if(ryInit < ry_min)
    {
        ryInit = ry_min;
        factor_y = static_cast<int>(floor(dy_p/ryInit));
    }

    ry = static_cast<int>(round(dy_p/((double)factor_y)));
    ry_p = ry;

    double words = s*rx_p*ry_p*rz_p;

    if(words > cacheWords)
    {
        if(dim>2)
        {
            //need to adjust rx or rz
            //first preference rx since rz is inner dimension
            double rxInit = cacheWords/(s*ry_p*rz_p); //rz_p == dz_p

            //Used ceil so if possible to avoid decomposing z direction
            int factor_x = static_cast<int>(ceil(dx_p/rxInit));

            if(rxInit < rx_min)
            {
                rxInit = rx_min;
                factor_x = static_cast<int>(floor(dx_p/rxInit));
            }

            //Rounding it to get lower reminder
            rx = static_cast<int>(round(dx_p/((double)factor_x)));
            rx_p = rx;

            words = s*rx_p*ry_p*rz_p;
        }

        //now adjust rz if required
        if(words > cacheWords)
        {
            double rzInit = cacheWords/(s*rx_p*ry_p);

            if(ryInit < ry_min)
            {
                rzInit = rz_min;
            }

            //Here used ceil; since now I can't do much
            int factor_z = static_cast<int>(ceil(dz_p/rzInit));

            rz = static_cast<int>(round(dz_p/((double)factor_z)));
            rz_p = rz;

            words = s*rx_p*ry_p*rz_p;

            if(words > cacheWords)
            {
                WARNING_PRINT("Temporal blocking might not be helpful please do it manually if required");
                return false;
            }
        }
    }

    //make rt good multiple of nthreads
    //1. find multiples of nthreads
    std::vector<int> outer_threads;
    std::vector<int> middle_threads;
    std::vector<int> inner_threads;
    for(int i=nthreads; i>=1; --i)
    {
        int out=i;
        int rest=nthreads/i;

        if(out*rest == nthreads)
        {
            for(int j=rest; j>=1; --j)
            {
                int middle=j;
                int in = rest/j;

                if(middle*in == rest)
                {
                    outer_threads.push_back(out);
                    middle_threads.push_back(middle);
                    inner_threads.push_back(in);
                }
            }
        }
    }

    double radius_x, radius_y;
    getRadiusFolded(radius_x, this, x);
    getRadiusFolded(radius_y, this, y);

    bx_min = std::min(rx, std::max(5,static_cast<int>(3*radius_x)));
    by_min = std::min(ry, std::max(5,static_cast<int>(3*radius_y)));
    bz_min = std::min(rz, 64 + 8*static_cast<int>(fold_z/4.0)*fold_z);

    for(int i=0; i<(int)outer_threads.size(); ++i)
    {
        printf("checking %d x %d x %d\n", outer_threads[i], middle_threads[i], inner_threads[i]);
        int out_rem = rx%outer_threads[i];
        int mid_rem = ry%middle_threads[i];
        int in_rem = rz%inner_threads[i];

        if((out_rem < 6) && (mid_rem < 12) && (in_rem < 20))
        {
            if( (((rx-out_rem)>=rx_min) && ((ry-mid_rem)>=ry_min)) && ((rz-in_rem)>=rz_min) )
            {
                if( ((rx-out_rem)/outer_threads[i] >= bx_min) && ((ry-mid_rem)/middle_threads[i] >= by_min) && ((rz-in_rem)/inner_threads[i] >= bz_min) )
                {
                    rx = rx-out_rem;
                    rx_p = rx;
                    ry = ry-mid_rem;
                    ry_p = ry;
                    rz = rz-in_rem;
                    rz_p = rz;

                    break;
                }
            }
        }
    }
    //Right now we use all the time available for
    //blocking; might need to restrict it;
    //to say max 50;
    //int factor_t = static_cast<int>(dt/100.0);
    rt = dt; //std::min(dt, dt/factor_t);

    INFO_PRINT("Found: rt=%d rx=%d ry=%d, rz=%d", rt, rx, ry, rz);
    return true;

}

bool yaskSite::blockTuner(char* temporalCache, char* spatialOBC, char* spatialIBC, double sf_temporal, double sf_OBC, double sf_IBC)
{
    if(temporalTuner(temporalCache, sf_temporal))
    {
        std::vector<int> retThreads = spatialTuner(spatialOBC, spatialIBC, sf_OBC, sf_IBC, temporalCache);

        if(retThreads[0]==1)
        {
            int new_rz = retThreads[3]*bz;
            int new_ry = retThreads[2]*by;
            int new_rx = retThreads[1]*bx;

            if(std::abs(new_rz-dz_p)<10)
            {
                new_rz=rz;
            }
            if(std::abs(new_ry-dy_p)<10)
            {
                new_ry=ry;
            }
            if(std::abs(new_rx-dx_p)<10)
            {
                new_rx=rx;
            }

            //make regional block multiple of block and nthreads
            setRegion(new_rz, new_ry, new_rx, dt, false);
            return true;
        }
    }
    return false;
}



//TODO pinning strategy
#define RUN_CMD "cd %s && OMP_NUM_THREADS=%d bin/yask.sh -stencil %s -arch %s -exe_prefix 'taskset -c %d-%d' -block_threads %d -dt %d -dx %d -dy %d -dz %d -bx %d -by %d -bz %d -sbx %d -sby %d -sbz %d && cd -", localDir, nthreads, stencil, arch, 0, nthreads-1, threadPerBlock, dt, dx, dy, dz, bx, by, bz, sbx, sby, sbz
/*
void yaskSite::runBench()
{
    SYSTEM(sysLogFileName, RUN_CMD);
}*/


bool yaskSite::needUpdate()
{
    if( ( (dimUpdated)||(regionUpdated) )|| ((blockUpdated) || ((subBlockUpdated) || threadUpdated)) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void yaskSite::assignUpdate(bool val)
{
    threadUpdated=val;
    dimUpdated=val;
    regionUpdated=val;
    blockUpdated=val;
    subBlockUpdated=val;
}

void yaskSite::setYASK_AT(bool enable)
{
    dynYASK_AT(this, enable);
}

void yaskSite::init(bool noAlloc)
{
    if(needUpdate())
    {
        if(!stencilReady)
        {
            build();
        }

        if(dynInit(this, noAlloc)<0)
        {
            ERROR_PRINT("Stencil is not generated");
        }

        //set back all to false
        assignUpdate(false);

        rankedEqGroups.clear();
        for(int i=0; i<num_eqns; ++i)
        {
            rankedEqGroups.push_back(i);
        }

        //find important eq. groups; rank them; sorting it might lead to
        //difficulties in finding proper assembly code
        std::stable_sort(rankedEqGroups.begin(), rankedEqGroups.end(), [&](const int& a, const int& b) {return (eqGroups[a].num_points > eqGroups[b].num_points);} );

        double total_spatial_size = dx*dy*dz;
        for(int i=numMainEqns; i<num_eqns; ++i)
        {
            //give warning if neglected eqn Group has more than 10% contribution
            if( (eqGroups[rankedEqGroups[i]].num_points/total_spatial_size)*100.0 > 10 )
            {
                WARNING_PRINT("Eq. Group %s neglected for perf. prediction; but has a contribution of more than 10%", eqGroups[rankedEqGroups[i]].name.c_str());
            }
        }

        //to get eqGroup and asm relation the order of main eqGroups has to be
        //in unpermuted order
        mainEqGroups.clear();
        for(int i=0; i<numMainEqns; ++i)
        {
            mainEqGroups.push_back(rankedEqGroups[i]);
        }

        std::stable_sort(mainEqGroups.begin(), mainEqGroups.end());

        //Now tie mainEqGroup with their perf. model
        for(int i=0; i<numMainEqns; ++i)
        {
            models[i]->setReadWriteGrids(eqGroups[mainEqGroups[i]].num_spatial_reads, eqGroups[mainEqGroups[i]].num_spatial_writes,eqGroups[mainEqGroups[i]].num_spatial_read_write, eqGroups[mainEqGroups[i]].num_stencils);
            models[i]->setWeight( (eqGroups[mainEqGroups[i]].num_points / (double) total_spatial_size) );
            PRINT_LOG("eqG = %d, weight = %f\n", i, models[i]->getWeight());
            eqGroups[mainEqGroups[i]].model = models[i];
        }
    }

    //disable YASK's auto tuner
    setYASK_AT(false);
}

//This function will create individual *.o files by linking to
//appropriate generated files
void yaskSite::run()
{
    if(needUpdate())
    {

        init();
    }

    if(yaskSite_VERBOSITY > 2)
    {
        INFO_PRINT("Running %s : nthreads=%d, dim='t=%d,x=%d,y=%d,z=%d' region='t=%d,x=%d,y=%d,z=%d' block='x=%d,y=%d,z=%d' subBlock='x=%d,y=%d,z=%d'", stencilCode, nthreads, dt,dx,dy,dz, rt,rx,ry,rz, bx,by,bz, sbx,sby,sbz);
    }

    if( dz==-1 )
    {
        ERROR_PRINT("ERROR: It seems you haven't set the dimension\n");
    }

#ifdef ENABLE_LOOPADAPT
    printf("calling loop adapt begin\n");
    loop_adapt_begin(totalTime, 0);
#endif

#ifdef LIKWID_PERFMON
#pragma omp parallel
    {
        LIKWID_MARKER_START(stencilCode);
    }
#endif

    //totalTime is offseted
    if(dynStencil(this, totalTime)<0)
    {
        ERROR_PRINT("Stencil is not generated");
    }

#ifdef LIKWID_PERFMON
#pragma omp parallel
    {
        LIKWID_MARKER_STOP(stencilCode);
    }
#endif

#ifdef ENABLE_LOOPADAPT
    loop_adapt_end(totalTime, 0);
#endif

    totalTime+=dt;

    onceRun = true;
}


/*only local part will be there in  output, array of size
 * dx*dy*dz has to be allocated and the array would contain
 * data in lexicographic ordering*/
int yaskSite::getOutput(double* out, const char* grid_name)
{
    if(!dp)
    {
        ERROR_PRINT("getOutput wrong argument : Pass a double pointer");
    }
    int idx = find_string(gridName, grid_name);
    if(idx<0)
    {
        ERROR_PRINT("The requested grid = %s does not exist", grid_name);
        return -1;
    }
    EXTRACT_IDX(gridIdx[idx]);

#pragma omp parallel for collapse(3)
    for(int x=x_start, loc_x=0; x<=x_end; ++x, ++loc_x)
    {
        for(int y=y_start, loc_y=0; y<=y_end; ++y, ++loc_y)
        {
            for(int z=z_start, loc_z=0; z<=z_end; ++z, ++loc_z)
            {
                out[loc_x*dy*dz+loc_y*dz+loc_z] = this->getOutputAt(gridName[idx],x,y,z);
            }
        }
    }
    return 1;
}


/*only local part will be there in  output, array of size
 * dx*dy*dz has to be allocated and the array would contain
 * data in lexicographic ordering*/
int yaskSite::getOutput(float* out, const char* grid_name)
{
    if(dp)
    {
        ERROR_PRINT("getOutput wrong argument : Pass a float pointer");
    }
    int idx = find_string(gridName, grid_name);
    if(idx<0)
    {
        ERROR_PRINT("The requested grid = %s does not exist", grid_name);
        return -1;
    }
    EXTRACT_IDX(gridIdx[idx]);

#pragma omp parallel for collapse(3)
    for(int x=x_start, loc_x=0; x<=x_end; ++x, ++loc_x)
    {
        for(int y=y_start, loc_y=0; y<=y_end; ++y, ++loc_y)
        {
            for(int z=z_start, loc_z=0; z<=z_end; ++z, ++loc_z)
            {
                out[loc_x*dy*dz+loc_y*dz+loc_z] = (float)(this->getOutputAt(gridName[idx],x,y,z));
            }
        }
    }
    return 1;
}

/*input array has to be of size dx*dy*dz and should contain
 * elements to be set in lexicographic ordering*/
int yaskSite::setInputScalar(double in, const char* grid_name)
{
    int idx = find_string(gridName, grid_name);
    bool all = false;

    if(idx < 0)
    {
        if(strcmp(grid_name, "all")==0)
        {
            all = true;
        }
        else
        {
            ERROR_PRINT("The requested grid = %s does not exist", grid_name);
            return -1;
        }
    }

    std::vector<int> selectedIdx;

    if(all)
    {
        for(unsigned int i=0; i<gridName.size(); ++i)
        {
            selectedIdx.push_back(i);
        }
    }
    else
    {
        selectedIdx.push_back(idx);
    }

    for(unsigned int i=0; i<selectedIdx.size(); ++i)
    {
        this->dynSetAll(this, gridName[selectedIdx[i]], in);
    }
    return 1;
}

/*input array has to be of size dx*dy*dz and should contain
 * elements to be set in lexicographic ordering*/
int yaskSite::setInput(double* in, const char* grid_name)
{
    int idx = find_string(gridName, grid_name);
    bool all = false;

    if(idx < 0)
    {
        if(strcmp(grid_name, "all")==0)
        {
            all = true;
        }
        else
        {
            ERROR_PRINT("The requested grid = %s does not exist", grid_name);
            return -1;
        }
    }

    std::vector<int> selectedIdx;

    if(all)
    {
        for(unsigned int i=0; i<gridName.size(); ++i)
        {
            selectedIdx.push_back(i);
        }
    }
    else
    {
        selectedIdx.push_back(idx);
    }

    for(unsigned int i=0; i<selectedIdx.size(); ++i)
    {
        EXTRACT_IDX(gridIdx[idx]);


#pragma omp parallel for collapse(3)
        for(int x=x_start, loc_x=0; x<=x_end; ++x, ++loc_x)
        {
            for(int y=y_start, loc_y=0; y<=y_end; ++y, ++loc_y)
            {
                for(int z=z_start, loc_z=0; z<=z_end; ++z, ++loc_z)
                {
                    this->setInputAt(in[loc_x*dy*dz+loc_y*dz+loc_z],gridName[selectedIdx[i]],x,y,z);
                }
            }
        }
    }

    return 1;
}


/*input array has to be of size dx*dy*dz and should contain
 * elements to be set in lexicographic ordering*/
int yaskSite::setInput(float* in, const char* grid_name)
{
    int idx = find_string(gridName, grid_name);
    bool all = false;

    if(idx < 0)
    {
        if(strcmp(grid_name, "all")==0)
        {
            all = true;
        }
        else
        {
            ERROR_PRINT("The requested grid = %s does not exist", grid_name);
            return -1;
        }
    }

    std::vector<int> selectedIdx;

    if(all)
    {
        for(unsigned int i=0; i<gridName.size(); ++i)
        {
            selectedIdx.push_back(i);
        }
    }
    else
    {
        selectedIdx.push_back(idx);
    }

    for(unsigned int i=0; i<selectedIdx.size(); ++i)
    {
        EXTRACT_IDX(gridIdx[idx]);


#pragma omp parallel for collapse(3)
        for(int x=x_start, loc_x=0; x<=x_end; ++x, ++loc_x)
        {
            for(int y=y_start, loc_y=0; y<=y_end; ++y, ++loc_y)
            {
                for(int z=z_start, loc_z=0; z<=z_end; ++z, ++loc_z)
                {
                    this->setInputAt(in[loc_x*dy*dz+loc_y*dz+loc_z],gridName[selectedIdx[i]],x,y,z);
                }
            }
        }
    }

    return 1;
}

void* yaskSite::getGridPtr(const char* grid_name)
{
    int idx = find_string(gridName, grid_name);

    if(idx<0)
    {
        ERROR_PRINT("%s grid not found\n", grid_name);
        return NULL;
    }

    void* ptr = dynGetGridPtr(this, grid_name);
    return ptr;
}

Grid yaskSite::operator[] (char* grid_name)
{
    return Grid(this,grids[gridMap[grid_name]]);
}

int yaskSite::getHaloLayer(const char* grid, int dim_)
{
    return dynGetHalo(this, grid, dim_);
}


//checks for compatibility between 2 stencils
//o/p : 0 if not compatible
//    : 1 dim compatible but data not compatible (requires copying)
//    : 2 dim and data compatible (one could move data)
//    Right now dt need not be compatible
int yaskSite::checkCompatibility(yaskSite* otherStencil)
{
    if( (dx!=otherStencil->dx) || ((dy!=otherStencil->dy) || (dz!=otherStencil->dz)) )
    {
        return 0;
    }
    else
    {
        if( ( (strcmp(path, otherStencil->path)!=0) || (fold_x!=otherStencil->fold_x) ) || ( (fold_y!=otherStencil->fold_y) || (fold_z!=otherStencil->fold_z) ) )
        {
            return 1;
        }
    }
    return 2;
}

/*The paramter will be added only once the kernel is created*/
double yaskSite::getParam(const char* param_name)
{
    int idx = find_string(paramName, param_name);
    if(idx < 0)
    {
        ERROR_PRINT("Paramter %s not found\n Available parameters are:", param_name);
        for(int i=0; i<((int)paramName.size());++i)
        {
            printf("%d. %s\n", i+1, paramName[i]);
        }

        return -1;
    }

    return getElement(param_name);
}

void yaskSite::setParam(double val, const char* param_name)
{
    int idx = find_string(paramName, param_name);
    if(idx < 0)
    {
        ERROR_PRINT("Paramter %s not found\n Available parameters are:", param_name);
        for(int i=0; i<((int)paramName.size());++i)
        {
            printf("%d. %s\n", i+1, paramName[i]);
        }

    }
    else
    {
        setElement(val, param_name);
    }
}


void yaskSite::calcECM(bool validate)
{
    //model->printECM(validate);
    for(int i=0; i<numMainEqns; ++i)
    {
        //Currently no validation for individual eqn. Group
        eqGroups[mainEqGroups[i]].model->model(1,false);
    }

    overallModel = *(eqGroups[mainEqGroups[0]].model);
    overallModel.setDerived(true);

    //calculate overallModel
    for(int i=1; i<numMainEqns; ++i)
    {
        overallModel = overallModel + *(eqGroups[mainEqGroups[i]].model);
    }

    printf(".........going to validate\n");
    overallModel.model(1, validate);
    printf("validation done.........\n");
}


void yaskSite::printECM()
{
    for(int i=0; i<numMainEqns; ++i)
    {
        eqGroups[mainEqGroups[i]].model->printECM(eqGroups[mainEqGroups[i]].name.c_str());
    }

    overallModel.printECM("overall");
}

std::vector<double> yaskSite::getECM(bool data)
{
    if(data)
    {
        return overallModel.ECM_data;
    }
    else
    {
        return overallModel.ECM;
    }
}

std::vector<double> yaskSite::getECM_boundary(bool data)
{
    if(data)
    {
        return overallModel.ECM_boundary;
    }
    else
    {
        return overallModel.ECM_boundary_cy;
    }
}

std::vector<double> yaskSite::getECM_prefetch(bool data)
{
    if(data)
    {
        return overallModel.ECM_prefetch;
    }
    else
    {
        return overallModel.ECM_prefetch_cy;
    }
}

std::vector<double> yaskSite::getECM_assoc(bool data)
{
    if(data)
    {
        return overallModel.ECM_assoc;
    }
    else
    {
        return overallModel.ECM_assoc_cy;
    }
}

std::vector<double> yaskSite::getECM_latency()
{
    return overallModel.ECM_latency;
}

std::vector<double> yaskSite::getECM_validate(bool data)
{
    if(data)
    {
        return overallModel.ECM_validate_data;
    }
    else
    {
        return overallModel.ECM_validate;
    }
}

double yaskSite::getPerfECM()
{
    return overallModel.getPerf();
}

double yaskSite::getECM_BurstPenalty()
{
    return overallModel.ECM_penalty;
}

double yaskSite::getCpuFreq()
{
    return cpu_freq;
}

std::vector<double> yaskSite::getSaturation()
{
    return overallModel.getSaturation();
}

void yaskSite::write2dFile(const char *filename, char* grid_name, char* dim_str, bool halo)
{
    std::ofstream F;
    F.open(filename);

    char* dim_str_cpy = strdup(dim_str);

    char* dim_dup = strdup(dim_str);
    char** splitted_dim = str_split(dim_dup, ':');

    std::vector<int> start(2);
    std::vector<int> end(2);

    for(int plot_dim=0; plot_dim<2; ++plot_dim)
    {
        if(strcmp(splitted_dim[plot_dim],"x")==0)
        {
            start[plot_dim] = halo?(-getHaloLayer(grid_name,0)):0;
            end[plot_dim] = halo?(dx+getHaloLayer(grid_name,0)):dx;
        }
        else if(strcmp(splitted_dim[plot_dim],"y")==0)
        {
            start[plot_dim] = halo?(-getHaloLayer(grid_name,1)):0;
            end[plot_dim] = halo?(dy+getHaloLayer(grid_name,1)):dy;
        }
        else
        {
            start[plot_dim] = halo?(-getHaloLayer(grid_name,2)):0;
            end[plot_dim] = halo?(dz+getHaloLayer(grid_name,2)):dz;
        }
    }

    for (int i = start[0]; i < end[0]; i++)
    {
        for (int j = start[1]; j < end[1]; j++)
        {
            double out = GET_PERM_OUT(dim_str_cpy,this,grid_name);
            F << i << "\t" << j << "\t" << out << std::endl;
        }
        F << std::endl;
    }
    F.close();
}

contribution yaskSite::getECMContributions()
{
    return overallModel.ecm_contribution;
}
