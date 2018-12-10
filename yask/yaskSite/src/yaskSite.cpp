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

#ifdef ENABLE_LOOPADAPT
    #include <loop_adapt.h>
#endif


//stencil_arch_foldz_foldy_foldx_path_likwid
#define LOCAL_DIR_NAME\
    "%s/%s_%s_%d_%d_%d_%s_%s", currPath, stencil, arch, fold_z, fold_y, fold_x, (strcmp(path_wo_space,"")==0)?"default":path_wo_space,(likwid)?"likwid":"noLikwid"

yaskSite::yaskSite(MPI_Manager* mpi_man_, char* stencil_, int dim_, int fold_z_, int fold_y_, int fold_x_, char* path_, bool likwid):mpi_man(mpi_man_), stencil(stencil_), arch(TARGET_ARCH), yaskDir(YASK_PATH), stencilCode(NULL), localDir(NULL), sysLogFileName(NULL), nthreads(1), threadPerBlock(1), fold_x(fold_x_), fold_y(fold_y_), fold_z(fold_z_), sbx(-1), sby(-1), sbz(-1), bx(-1), by(-1), bz(-1), rt(-1), rx(-1), ry(-1), rz(-1), dt(-1), dx(-1), dy(-1), dz(-1), totalTime(0), radius(1), dim(dim_), buildWithLikwid(likwid), path(path_), stencilReady(false), subBlockUpdated(false), blockUpdated(false), regionUpdated(false), dimUpdated(false), threadUpdated(false), onceRun(false), dl_handle(NULL), dynInit(&YASKinit), dynFinalize(&YASKfinalize), dynStencil(&YASKstencil), dynGetPtr(&YASKgetElPtr), stencilContext(NULL), stencilSettings(NULL)
{
    //get current absolute path
    char* currPath = get_current_dir_name();

    //remove space in path and replace by underScore
    char* path_wo_space = removeSpaces(path);
    STRINGIFY(localDir, LOCAL_DIR_NAME);
    delete[] path_wo_space;

    SYSTEM("mkdir -p %s/bin", localDir);

    STRINGIFY(sysLogFileName, "%s/sysCallLog.log", localDir);

    //copy yask.sh; this will be deprecated
    SYSTEM("cp %s/bin/yask.sh %s/bin/.", yaskDir, localDir);

    //stencil code
    STRINGIFY(stencilCode,"stencil_%s_%s_%d_%d_%d_%s", stencil, arch, fold_x, fold_y, fold_z, (strcmp(path,"")==0)?"default":path);

}

yaskSite::~yaskSite()
{
#ifndef CACHE_BUILD
    cleanDir();
#endif

    delete[] localDir;
    delete[] sysLogFileName;
    delete[] stencilCode;

    //if there is an active kernel
    //delete it
    if(stencilContext)
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
}

void yaskSite::systemCall(char* cmd)
{
    systemCallUtil(cmd, sysLogFileName);
}


void yaskSite::cleanDir()
{
    //remove the localDir
    SYSTEM("rm -r -f %s", localDir);
}


//added -fPIC for compiling as shared lib
#define BUILD_CMD\
    "make -C %s EXTRA_CXXFLAGS=-fPIC stencil=%s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=8 SUB_BLOCK_LOOP_OUTER_MODS=\"%s\" likwid=%d", yaskDir, stencil, arch, fold_x, fold_y, fold_z, path, (int) buildWithLikwid\

void yaskSite::build()
{
    SYNC_WITH_DIM(fold_z, fold_y, fold_x, "fold");

    char *lib;
    STRINGIFY(lib, "%s/lib/libYASK.so", localDir);

    //check if library already exists, if yes don't rebuild
    void* test_open = dlopen(lib, RTLD_LOCAL|RTLD_NOW);

    if(test_open == NULL)
    {
        //Need to newly build

        //TODO: check for previous builds
        //clenaing source build before doing anything
        cleanBuild();

        LOAD_PRINT_START("Building %s : stencil=%s arch=%s fold='x=%d,y=%d,z=%d' real_bytes=8 path=%s", stencilCode, stencil, arch, fold_x, fold_y, fold_z, (strcmp(path,"")==0)?"default":path);

        SYSTEM(BUILD_CMD);
        SYSTEM("cp %s/bin/yask.%s.%s.exe %s/bin/.", yaskDir, stencil, arch, localDir);

        //create YASK library
        SYSTEM("%s/createYASKLib.sh %s %s %s %s %s %s", TOOL_DIR, yaskDir, stencil, arch, SRC_DIR, INC_DIR, localDir);

        LOAD_PRINT_END();

        //dlopen and link them
        //generated library
        LOAD_PRINT_START("Loading %s", stencilCode);

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
    dynInit = (fn1_t) dlsym(dl_handle, "YASKinit");

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
    dynStencil = (fn1_t) dlsym(dl_handle, "YASKstencil");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKstencil");
    }

    //reset error
    dlerror();
    dynGetPtr = (fn2_t) dlsym(dl_handle, "YASKgetElPtr");
    if(dlerror())
    {
        ERROR_PRINT("Error couldn't find symbol with name YASKgetElPtr");
    }


    delete[] lib;
    stencilReady=true;
}

void yaskSite::cleanBuild()
{
    SYSTEM("make -C %s clean", yaskDir);
    SYSTEM( "rm -f %s/cxx-flags.%s.%s.txt %s/ld-flags.%s.%s.txt %s/make-report.%s.%s.txt", yaskDir, stencil, arch, yaskDir, stencil, arch, yaskDir, stencil, arch);
}

void yaskSite::setSubBlock(int z, int y, int x)
{
    SYNC_WITH_DIM(z, y, x, "subBlock");
    sbx=x;
    sby=y;
    sbz=z;
    subBlockUpdated = true;
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

    rt = dt;
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
}


void yaskSite::setDefaultBlock()
{
    blockUpdated = true;
    if(dim==1)
    {
        bx = 1;
        by = 1;
        bz = static_cast<int>(round(threadPerBlock*rz/((double)nthreads)));
    }
    else if(dim==2)
    {
        bx = 1;
        //Similar to OMP on outer-most loop
        //TODO inner shouldn't in priciple hurt in yask if loop size is big
        //enough
        by = static_cast<int>(round(threadPerBlock*ry/((double)nthreads)));
        bz = rz;
    }
    else if(dim==3)
    {
        //Loop to parallelise can be selected by benchmarks, may be hybrid is
        //good in this case but I wouldn't do this here,
        //To get good values there are 2 options:
        //1. use loopTuner
        //2. manually set Block and subBlock
        //default outer loop will be parallelised
        bx = static_cast<int>(round(threadPerBlock*rx/((double)nthreads)));
        by = ry;
        bz = rz;
    }
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
    (ry*rz)/(n*b_)\

bool yaskSite::spatialTuner(char* OBC_str, char* IBC_str, double sf_OBC, double sf_IBC)
{
    if(dim==1)
    {
        return true;
    }

    else if(dim==2)
    {
        bx=1;
        by=ry;

        double layer = (2*radius+1+1); //layer width
        cache_info OBC = cache(OBC_str);
        double OBC_words = (OBC.shared)?(sf_OBC*OBC.words)/nthreads:(sf_OBC*OBC.words);

        double n_z = (layer*((double)rz))/(OBC_words);
        //round n_z to next greatest multiple of nthreads
        double n = ceil(n_z/nthreads)*nthreads;

        if(n_z>1)
        {
            bz = static_cast<int>(round(rz/n));
            setDefaultSubBlock();
            INFO_PRINT("Found: bx=%d by=%d, bz=%d", bx, by, bz);
        }
        else
        {
            setDefaultBlock();
            setDefaultSubBlock();
            INFO_PRINT("Spatial Blocking not carried out, dim fits in cache");
        }

        return true;
    }
    else
    {
        //don't make bz very small since its the inner dimension
        int bz_min = 128;
        int by_min = 4;

        bool blocked = false;

        double layer = (2*radius+1+1); //layer width
        cache_info OBC = cache(OBC_str);
        cache_info IBC = cache(IBC_str);

        //TODO: determine how many threads are currently sharing the resources
        //if its in shared mode
        double OBC_words = (OBC.shared)?(sf_OBC*OBC.words)/nthreads:(sf_OBC*OBC.words);
        double IBC_words = (IBC.shared)?(sf_IBC*IBC.words)/nthreads:(sf_IBC*IBC.words);

        double n_z = (layer*((double)ry)*((double)rz))/(OBC_words);
        //round n_z to next greatest multiple of nthreads
        double n = ceil(n_z/nthreads)*nthreads;
        double bz_start = std::min(IBC_words/layer, (double)(rz));
        double bz_end = bz_min;

        //find bz_start: such that its perfect multiples of rz (to avoid reminder)
        int bz_ratio_start = static_cast<int>(ceil(rz/bz_start));
        int bz_ratio_end   = static_cast<int>(floor(rz/bz_end));

        double bz_tune, by_tune;

        if(n_z > 1)
        {
            for(int bz_ratio = bz_ratio_start; bz_ratio<= bz_ratio_end; ++bz_ratio)
            {
                //now get correct bz_start and bz_end; with perfect multiples of rz
                bz_tune = rz/(static_cast<double>(bz_ratio));
                by_tune = f(static_cast<double>(bz_tune));

                if(by_tune > by_min)
                {
                    blocked = true;
                    break;
                }
            }

            if(!blocked)
            {
                WARNING_PRINT("Spatial Blocking might not be helpful; please block manually if needed");
            }
            else
            {
                bx = rx;
                bz = static_cast<int>(round(bz_tune));
                by = static_cast<int>(round(by_tune));
                //now set subBlocks, based on by and bz
                setDefaultSubBlock();

                INFO_PRINT("Found: bx=%d by=%d, bz=%d", bx, by, bz);
            }
        }
        else
        {
            INFO_PRINT("Spatial Blocking not carried out, dim fits in cache");
            setDefaultBlock();
            setDefaultSubBlock();
            blocked = true;
        }

        return blocked;
    }
}

bool yaskSite::temporalTuner(char  *cacheStr, double sf)
{
    int rx_min = 30;
    int ry_min = 10;
    int rz_min = 128;

    if(dim<3)
    {
        ry_min = 128;
        rz_min = 512;
    }

    cache_info blockCache = cache(cacheStr);
    if(!blockCache.shared)
    {
        WARNING_PRINT("Will not do any tuning for temporal blocking; because %s is not a shared cache", cacheStr);
    }

    //TODO: determine how many threads share the cache
    //maybe use hwloc
    double cacheWords = sf*blockCache.words;

    //2 because 2 arrays; TODO if more arrays are there
    double ryInit = cacheWords/((double)(2.0*rx*rz));
    //now set ry : ry is a proper multiple of dy; used ceil to absorb
    //if possible all the required decomposition here
    int factor_y = static_cast<int>(ceil(dy/ryInit));


    //reminders at regions might affect performance; since blocks at reminder
    //region cannot be distributed to threads properly
    if(ryInit < ry_min)
    {
        ryInit = ry_min;
        factor_y = static_cast<int>(floor(dy/ryInit));
    }

    ry = static_cast<int>(round(dy/((double)factor_y)));

    double words = 2.0*rx*ry*rz;


    if(words > cacheWords)
    {
        if(dim>2)
        {
            //need to adjust rx or rz
            //first preference rx since rz is inner dimension
            double rxInit = cacheWords/(2.0*ry*rz);

            //Used ceil so if possible to avoid decomposing z direction
            int factor_x = static_cast<int>(ceil(dx/rxInit));

            if(rxInit < rx_min)
            {
                rxInit = rx_min;
                factor_x = static_cast<int>(floor(dx/rxInit));
            }

            //Rounding it to get lower reminder
            rx = static_cast<int>(round(dx/((double)factor_x)));


            words = 2.0*rx*ry*rz;
        }

        //now adjust ry if required
        if(words > cacheWords)
        {
            double rzInit = cacheWords/(2.0*rx*ry);

            if(ryInit < ry_min)
            {
                rzInit = rz_min;
            }

            //Here used ceil; since now I can't do much
            int factor_z = static_cast<int>(ceil(dz/rzInit));

            rz = static_cast<int>(round(dz/((double)factor_z)));

            words = 2.0*rx*ry*rz;

            if(words > cacheWords)
            {
                WARNING_PRINT("Temporal blocking might not be helpful please do it manually if required");
                return false;
            }
        }
    }

    //Right now we use all the time available for 
    //blocking; might need to restrict it;
    //to say max 50;
    rt = dt;

    INFO_PRINT("Found: rt=%d rx=%d ry=%d, rz=%d", rt, rx, ry, rz);
    return true;
}

bool yaskSite::blockTuner(char* temporalCache, char* spatialOBC, char* spatialIBC, double sf_temporal, double sf_OBC, double sf_IBC)
{
    if(temporalTuner(temporalCache, sf_temporal))
    {
        if(spatialTuner(spatialOBC, spatialIBC, sf_OBC, sf_IBC))
        {
            return true;
        }
    }
    return false;
}



//TODO pinning strategy
#define RUN_CMD "cd %s && OMP_NUM_THREADS=%d bin/yask.sh -stencil %s -arch %s -exe_prefix 'taskset -c %d-%d' -block_threads %d -dt %d -dx %d -dy %d -dz %d -bx %d -by %d -bz %d -sbx %d -sby %d -sbz %d && cd -", localDir, nthreads, stencil, arch, 0, nthreads-1, threadPerBlock, dt, dx, dy, dz, bx, by, bz, sbx, sby, sbz

void yaskSite::runBench()
{
    SYSTEM(RUN_CMD);
}

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

void yaskSite::init()
{
    if(needUpdate())
    {
        if(!stencilReady)
        {
            build();
        }

        if(dynInit(this)<0)
        {
            ERROR_PRINT("Stencil is not generated");
        }

        //set back all to false
        assignUpdate(false);
    }
}

//This function will create individual *.o files by linking to
//appropriate generated files
void yaskSite::run()
{
    if(needUpdate())
    {
        init();
    }

    INFO_PRINT("Running %s : nthreads=%d, dim='t=%d,x=%d,y=%d,z=%d' region='t=%d,x=%d,y=%d,z=%d' block='x=%d,y=%d,z=%d' subBlock='x=%d,y=%d,z=%d'", stencilCode, nthreads, dt,dx,dy,dz, rt, rx, ry, rz, bx,by,bz, sbx,sby,sbz);


    if( dz==-1 )
    {
        ERROR_PRINT("ERROR: It seems you haven't set the dimension\n");
    }

#ifdef ENABLE_LOOPADAPT
   printf("calling loop adapt begin\n");
   loop_adapt_begin(totalTime, 0);
#endif

    if(dynStencil(this)<0)
    {
        ERROR_PRINT("Stencil is not generated");
    }

#ifdef ENABLE_LOOPADAPT
    loop_adapt_end(totalTime, 0);
#endif

    printf("totalTime = %d\n", totalTime);
    totalTime+=dt;

    onceRun = true;
}

/*only local part will be there in  output, array of size
 * dx*dy*dz has to be allocated and the array would contain
 * data in lexicographic ordering*/
int yaskSite::getOutput(double* out, const char* grid_name)
{
    auto it = std::find(gridName.begin(), gridName.end(), grid_name);
    if(it==gridName.end())
    {
        ERROR_PRINT("The requested grid = %s does not exist", grid_name);
        return -1;
    }
    int idx = std::distance(gridName.begin(), it);

    EXTRACT_IDX(gridIdx[idx]);

#pragma omp parallel for collapse(3)
    for(int x=x_start, loc_x=0; x<=x_end; ++x, ++loc_x)
    {
        for(int y=y_start, loc_y=0; y<=y_end; ++y, ++loc_y)
        {
            for(int z=z_start, loc_z=0; z<=z_end; ++z, ++loc_z)
            {
                out[loc_x*dy*dz+loc_y*dz+loc_z] = this->getOutputAt(gridName[idx],x,y,z,false);
            }
        }
    }
    return 1;
}

/*input array has to be of size dx*dy*dz and should contain
 * elements to be set in lexicographic ordering*/
int yaskSite::setInput(double* in, const char* grid_name)
{
    auto it = std::find(gridName.begin(), gridName.end(), grid_name);
    if(it==gridName.end())
    {
        ERROR_PRINT("The requested grid = %s does not exist", grid_name);
        return -1;
    }
    int idx = std::distance(gridName.begin(), it);

    EXTRACT_IDX(gridIdx[idx]);

#pragma omp parallel for collapse(3)
    for(int x=x_start, loc_x=0; x<=x_end; ++x, ++loc_x)
    {
        for(int y=y_start, loc_y=0; y<=y_end; ++y, ++loc_y)
        {
            for(int z=z_start, loc_z=0; z<=z_end; ++z, ++loc_z)
            {
                this->setInputAt(in[loc_x*dy*dz+loc_y*dz+loc_z],gridName[idx],x,y,z,false);
            }
        }
    }
    return 1;
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

//copies current data of other stencil's newest calculation to this stencil's t0
//so the new stencil can continue executing the same problem
int yaskSite::transferData(yaskSite* stencilOther, char* data)
{
    if(needUpdate())
    {
        init();
    }

    //check for compatibility between stencil
    int compatible = checkCompatibility(stencilOther);
    if(compatible == 0)
    {
        ERROR_PRINT("Can't transfer data between stencils");
        return 0;
    }

    int grid_start, grid_end, other_grid_start;

    //grids give idx to each grid
    //and creates a vector of strings
    if(strcmp(data, "all")==0)
    {
        grid_start=0;
        other_grid_start=grid_start;
        grid_end=gridName.size();

        //check if both grid names are equal; else quit
        for(int i=grid_start; i<grid_end; ++i)
        {
            if(strcmp(gridName[i],stencilOther->gridName[i])!=0)
            {
                ERROR_PRINT("The requested grid = %s does not exist in the stencil to be copied from", gridName[i]);
                return 0;
            }
        }

    }
    else
    {
        auto it = std::find(gridName.begin(), gridName.end(), data);
        if(it==gridName.end())
        {
            ERROR_PRINT("The requested grid = %s does not exist", data);
            return 0;
        }

        grid_start = std::distance(gridName.begin(), it);
        grid_end=grid_start+1;

        other_grid_start = std::distance(stencilOther->gridName.begin(), std::find(stencilOther->gridName.begin(), stencilOther->gridName.end(), data));
    }

    //find newest timestep to transfer;
    int t = (stencilOther->dt);

    for(int idx=grid_start, other_idx=other_grid_start; idx<grid_end; ++idx, ++other_idx)
    {
        EXTRACT_IDX(gridIdx[idx]);

#pragma omp parallel for collapse(3)
        for(int x=x_start; x<=x_end; ++x)
        {
            for(int y=y_start; y<=y_end; ++y)
            {
                for(int z=z_start; z<=z_end; ++z)
                {
                    *(this->getPtrAt(gridName[idx],0,x,y,z,false)) = *(stencilOther->getPtrAt(stencilOther->gridName[other_idx],t,x,y,z,false));
                }
            }
        }
    }

    return 1;
}
