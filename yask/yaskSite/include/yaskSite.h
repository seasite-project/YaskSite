#ifndef YASKSITE
#define YASKSITE

#include "config.h"
#include "macros.h"
#include "util.h"
#include "MPI_Manager.h"
#include <vector>

class yaskSite
{

    public:
        MPI_Manager* mpi_man;
        char* stencil;
        char* arch;
        char* yaskDir; //directory for building
        char* stencilCode;

        //local directory where executable exists to be run
        //for each object there exists such a directory, if build is executed
        //which could be later destroyed explicitly using cleanDir command.
        char* localDir;
        char* sysLogFileName;

        int nthreads;
        int threadPerBlock;

        int fold_x;
        int fold_y;
        int fold_z;

        int sbx;
        int sby;
        int sbz;

        int bx;
        int by;
        int bz;

        int rt;
        int rx;
        int ry;
        int rz;

        int dt;
        int dx;
        int dy;
        int dz;

        int totalTime; //total time steps done
        int radius;
        int dim;

        bool buildWithLikwid;
        //current options
        //1. default
        //2. grouped
        //3. serpentine
        char* path;
        bool stencilReady;
        bool subBlockUpdated;
        bool blockUpdated;
        bool regionUpdated;
        bool dimUpdated;
        bool threadUpdated;
        bool onceRun;

    private:
        //sets dimension dependent parameters
        //if user does not define
        void setDefaultRegion();
        void setDefaultBlock();
        void setDefaultSubBlock();
        bool needUpdate();
        void assignUpdate(bool val);

       //dl handle
        void* dl_handle;
        void build();
        void cleanBuild();
        void systemCall(char* cmd);

        //For YASK
        //dynamically loaded fns
        //fn type
        typedef int (*fn1_t) (yaskSite*);
        fn1_t dynInit;
        fn1_t dynFinalize;
        fn1_t dynStencil;
        typedef double* (*fn2_t) (yaskSite*,const char*,int,int,int,int,bool);
        fn2_t dynGetPtr;

        int checkCompatibility(yaskSite* otherStencil);

    public:
        //For YASK
        void* stencilContext;
        void* stencilSettings;
        std::vector<const char*> gridName;
        std::vector<std::vector<int>> gridIdx;

        //specify a architecture or will compile for current architecture
        yaskSite(MPI_Manager* mpi_man_, char* stencil_, int dim_, int fold_z_=1, int fold_y_=1, int fold_x_=1, char* path_="", bool likwid=false);

        ~yaskSite();
        void cleanDir();
        void setSubBlock(int z, int y, int x=1);
        void setBlock(int z, int y, int x=1, bool resetOthers=true);
        void setRegion(int z, int y, int x=1, int t=1, bool resetOthers=true);
        void setDim(int z, int y, int x=1, int t=1, bool resetOthers=true);
        void setThread(int nthreads_, int threadPerBlock_=1, bool resetOthers=true);

        //tunes block size and subBlock size
        bool spatialTuner(char* OBC_str, char* IBC_str="L2", double sf_OBC=0.5, double sf_IBC=0.5);

        bool temporalTuner(char* cache, double sf=0.5);

        bool blockTuner(char* temporalCache, char* spatialOBC, char* spatialIBC="L2", double sf_temporal = 0.5, double sf_OBC = 0.5, double sf_IBC =0.5);

        //z is the inner dimension, x is the outer dimension
        //run for benchmarking; needed for auto-tuning
        void runBench();

        //init creates proper domain decomposition
        //and makes a new context for YASK to run properly
        void init();
        void run();

        //access fns
        inline double* getPtrAt(const char* grid_name, int t, int x, int y, int z, bool checkBounds=false)
        {
             return (dynGetPtr(this, grid_name, t, x, y, z, checkBounds));
        }

        /*Specialized such that only data from newest grid is the output*/
        inline double getOutputAt(const char* grid_name, int x, int y, int z, bool checkBounds=false)
        {
            return (*dynGetPtr(this, grid_name, dt, x, y, z, checkBounds));
        }

        inline void setInputAt(double val, const char* grid_name, int x, int y, int z, bool checkBounds=false)
        {
            (*dynGetPtr(this, grid_name, 0, x, y, z, checkBounds)) = val;
        }

        int getOutput(double* val, const char* grid_name);
        int setInput(double* val, const char* grid_name);
        int transferData(yaskSite* stencilOther, char* data="all");
};

#endif
