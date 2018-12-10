#ifndef YASKSITE
#define YASKSITE

#include "config.h"
#include "macros.h"
#include "util.h"
#include "MPI_Manager.h"
#include <vector>
#include "codeGen.h"
#include "types.h"
#include "perfModel.h"
#include <string.h>


struct Grid;


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

        /*padded dimension
         * for good spatial
         * tuning*/
        int rx_p;
        int ry_p;
        int rz_p;

        int dx_p;
        int dy_p;
        int dz_p;


        int s; //If there is dependency within a stencil;
               // a single sweep of stencil calculates for 's' timesteps

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
        bool prefetch;
        bool cacheStale;
        STENCIL* stencilDetails;
        bool STENCIL_struct_alloc;

        std::map<std::string, int> gridMap;
        std::vector<GRID> grids;

        int num_eqns;
        int numMainEqns; //eqns to perf model
        std::map<std::string, int> eqGroupMap;
        std::vector<EQ_GROUP> eqGroups;
        std::vector<int> rankedEqGroups;
        std::vector<int> mainEqGroups;

    private:
        void initStencil(MPI_Manager* mpi_man_, char* stencilName_, int dim_, int radius, int fold_z_, int fold_y_, int fold_x_, bool prefetch, char* path_, bool likwid);

        //sets dimension dependent parameters
        //if user does not define
        void setDefaultRegion();
        void setDefaultBlock();
        bool setDefaultBlock_min_rem(int n_scale_down_olc=1, int n_scale_down_ilc=1);
        void setDefaultSubBlock();
        bool needUpdate();
        void assignUpdate(bool val);

       //dl handle
        void* dl_handle;
        void build();
        void cleanBuild();

        //For YASK
        //dynamically loaded fns
        //fn type
        typedef int (*fn1_t) (yaskSite*);
        fn1_t dynInit;
        fn1_t dynFinalize;
        typedef int (*fn2_t) (yaskSite*,int);
        fn2_t dynStencil;
        typedef double* (*fn3_t) (yaskSite*,const char*,int,int,int,int,bool);
        fn3_t dynGetPtr;
        typedef void* (*fn4_t) (yaskSite*,const char*);
        fn4_t dynGetGridPtr;
        typedef void (*fn5_t) (yaskSite*,const char*, void*);
        fn5_t dynSetGridPtr;
        typedef int (*fn6_t) (yaskSite*,const char*, int);
        fn6_t dynGetHalo;
        typedef void (*fn7_t) (yaskSite*,const char*, int, int);
        fn7_t dynSetHalo;

        int checkCompatibility(yaskSite* otherStencil);

        std::vector<perfModel*> models;
        perfModel overallModel;

    public:
        //For YASK
        void* stencilContext;
        void* stencilSettings;
        std::vector<const char*> gridName;
        std::vector<std::vector<int>> gridIdx;
        std::vector<const char*> paramName;
        std::vector<double*> paramList;

        //TODO specify a architecture or will compile for current architecture
        yaskSite(MPI_Manager* mpi_man_, STENCIL* stencil); //preferred constructor
        yaskSite(MPI_Manager* mpi_man_, char* stencilName_, int dim_, int radius=-1, int fold_z_=1, int fold_y_=1, int fold_x_=1, bool prefetch=false, char* path_="", bool likwid=false);
        ~yaskSite();
        void cleanDir();
        void setS(int s_val);
        void setSubBlock(int z, int y, int x=1);
        void setBlock(int z, int y, int x=1, bool resetOthers=true);
        void setRegion(int z, int y, int x=1, int t=1, bool resetOthers=true);
        void setDim(int z, int y, int x=1, int t=1, bool resetOthers=true);
        void setThread(int nthreads_, int threadPerBlock_=1, bool resetOthers=true);

        //tunes block size and subBlock size
        bool spatialTuner(char* OBC_str, char* IBC_str="L2", double sf_OBC=-1, double sf_IBC=-1);

        bool temporalTuner(char* cache, double sf=-1);

        bool blockTuner(char* temporalCache, char* spatialOBC, char* spatialIBC="L2", double sf_temporal = -1, double sf_OBC = -1, double sf_IBC = -1);

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
            return (*dynGetPtr(this, grid_name, totalTime, x, y, z, checkBounds));
        }

        inline void setInputAt(double val, const char* grid_name, int x, int y, int z, bool checkBounds=false)
        {
            (*dynGetPtr(this, grid_name, totalTime, x, y, z, checkBounds)) = val;
        }

        int getOutput(double* val, const char* grid_name);
        int setInputScalar(double val, const char* grid_name, bool halo=false);
        int setInput(double* val, const char* grid_name);

        void* getGridPtr(const char* grid);
        Grid operator[] (char* grid_name);
        void setGridPtr(const char* grid, void* ptr);

        int getHaloLayer(const char *grid, int dim_);
        void setHaloLayer(const char *grid, int dim_, int val);

        int transferData(yaskSite* stencilOther, char* data, int ts=0);

        double* getParam(const char* param_name);


        void calcECM(bool validate=false);
        void printECM();
        contribution getECMContributions();
        std::vector<double> getECM(bool data=false);
        std::vector<double> getECM_validate(bool data=false);

        double getPerfECM();

        void write2dFile(const char *filename, char* grid_name, char* dim_str, bool halo);
};

struct Grid
{
    yaskSite* stencil;
    void* ptr;
    GRID grid_details; //offline grid

    Grid(yaskSite* stencil_, void* ptr_, GRID grid_details_):stencil(stencil_),ptr(ptr_),grid_details(grid_details_)
    {
    }

    Grid(const Grid& other):stencil(other.stencil),ptr(other.ptr),grid_details(other.grid_details)
    {
    }

    inline Grid& operator=(Grid& other)
    {
        stencil = other.stencil;
        ptr = other.ptr;
        grid_details = other.grid_details;
        return *this;
    }

    inline Grid operator[] (int ts)
    {
        if(ts==0)
        {
            return (*this);
        }
        else
        {
            std::ptrdiff_t ts_diff = ((char*)stencil->getPtrAt(grid_details.name.c_str(), ts, 0, 0, 0) - (char*)stencil->getPtrAt(grid_details.name.c_str(), 0, 0, 0, 0));
            if(ts_diff==0)
            {
                return (*this);
            }
            else
            {
                return Grid(stencil, (char*)ptr+ts_diff, grid_details);
            }
        }
    }
};

inline bool operator<<(Grid lhs, Grid rhs)
{
    lhs.ptr=rhs.ptr;
    lhs.stencil->setGridPtr(lhs.grid_details.name.c_str(), rhs.ptr);

    for(int dim_=0; dim_<3; ++dim_)
    {
        lhs.stencil->setHaloLayer(lhs.grid_details.name.c_str(), dim_, rhs.stencil->getHaloLayer(rhs.grid_details.name.c_str(), dim_));
    }

    return true;
}


#endif
