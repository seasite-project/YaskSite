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

int makeGoodBigNumber(int bigNum, int smallNum, int ctr, int inc=1);
int makeGoodNumber(int bigNum, int smallNum, int ctr);

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

        bool dp;

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

        int bx_min;
        int by_min;
        int bz_min;

        int s; //If there is dependency within a stencil;
               // a single sweep of stencil calculates for 's' timesteps

        int maxNumStencils;

        int totalTime; //total time steps done
        int radius;
        int dim;

        int thread_per_socket;
        double cpu_freq;

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
        void initStencil(MPI_Manager* mpi_man_, char* stencilName_, int dim_, int radius, int fold_z_, int fold_y_, int fold_x_, bool dp, bool prefetch, char* path_, bool likwid);

        //sets dimension dependent parameters
        //if user does not define
        void setDefaultRegion();
        void setDefaultBlock();
        std::vector<int> setDefaultBlock_min_rem(int n_scale_down_olc=1, int n_scale_down_ilc=1, char *temporal_str="MEM");
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
        typedef int (*fn0_t) (yaskSite*,bool);
        fn0_t dynInit;
        typedef int (*fn1_t) (yaskSite*);
        fn1_t dynFinalize;
        typedef int (*fn2_t) (yaskSite*,int);
        fn2_t dynStencil;
        typedef double (*fn3_t) (yaskSite*,const char*,int,int,int,int);
        fn3_t dynGetElement;
        typedef void (*fn4_t) (yaskSite*,const char*,int,int,int,int, double);
        fn4_t dynSetElement;
        typedef void (*fn5_t) (yaskSite*,const char*, double);
        fn5_t dynSetAll;
        typedef void* (*fn6_t) (yaskSite*,const char*);
        fn6_t dynGetGridPtr;
        typedef void (*fn7_t) (yaskSite*,const char*, yaskSite*, const char*);
        fn7_t dynFuseGrid;
        typedef int (*fn8_t) (yaskSite*,const char*, int);
        fn8_t dynGetHalo;
        typedef void (*fn9_t) (yaskSite*, bool);
        fn9_t dynYASK_AT;

        int checkCompatibility(yaskSite* otherStencil);

        std::vector<perfModel*> models;
        perfModel overallModel;

    public:
        //For YASK
        void* stencilFactory;
        void* stencilSoln;
        std::vector<const char*> gridName;
        std::vector<std::vector<int>> gridIdx;
        std::vector<const char*> paramName;
        std::vector<void*> paramList;

        //TODO specify a architecture or will compile for current architecture
        yaskSite(MPI_Manager* mpi_man_, STENCIL* stencil); //preferred constructor
        yaskSite(MPI_Manager* mpi_man_, char* stencilName_, int dim_, int radius=-1, int fold_z_=1, int fold_y_=1, int fold_x_=1, bool dp = true, bool prefetch=false, char* path_="", bool likwid=false);
        ~yaskSite();
        void cleanDir();
        void setS(int s_val);
        void setSubBlock(int z, int y, int x=1);
        void setBlock(int z, int y, int x=1, bool resetOthers=true);
        void setRegion(int z, int y, int x=1, int t=1, bool resetOthers=true);
        void setDim(int z, int y, int x=1, int t=1, bool resetOthers=true);
        void setThread(int nthreads_, int threadPerBlock_=1, bool resetOthers=true);

        void setYASK_AT(bool enable);
        //tunes block size and subBlock size
        std::vector<int> spatialTuner(char* OBC_str, char* IBC_str="L2", double sf_OBC=-1, double sf_IBC=-1, char* temporal_str="MEM");

        bool temporalTuner(char* cache, double sf=-1);

        bool blockTuner(char* temporalCache, char* spatialOBC, char* spatialIBC="L2", double sf_temporal = -1, double sf_OBC = -1, double sf_IBC = -1);

        //z is the inner dimension, x is the outer dimension
        //run for benchmarking; needed for auto-tuning
        void runBench();

        //init creates proper domain decomposition
        //and makes a new context for YASK to run properly
        void init(bool noAlloc=false);
        void run();

        //access fns
        inline double getElement(const char* grid_name, int t=-404, int x=-404, int y=-404, int z=-404)
        {
             return (dynGetElement(this, grid_name, t, x, y, z));
        }
        inline void setElement(double val, const char* grid_name, int t=-404, int x=-404, int y=-404, int z=-404)
        {
             dynSetElement(this, grid_name, t, x, y, z, val);
        }


        /*Specialized such that only data from newest grid is the output*/
        inline double getOutputAt(const char* grid_name, int x, int y, int z)
        {
            return getElement(grid_name, totalTime, x, y, z);

        }
        inline void setInputAt(double val, const char* grid_name, int x, int y, int z)
        {
            setElement(val, grid_name, totalTime, x, y, z);
        }

        inline void fuseGrid(const char* to_grid_name, yaskSite* fromStencil, const char* from_grid_name)
        {
            dynFuseGrid(this, to_grid_name, fromStencil, from_grid_name);
        }


        int getOutput(double* val, const char* grid_name);
        int getOutput(float* val, const char* grid_name);

        int setInputScalar(double val, const char* grid_name);

        int setInput(double* val, const char* grid_name);
        int setInput(float* val, const char* grid_name);

        void* getGridPtr(const char* grid);
        Grid operator[] (char* grid_name);

        int getHaloLayer(const char *grid, int dim_);

        double getParam(const char* param_name);
        void setParam(double val, const char* param_name);

        void calcECM(bool validate=false);
        void printECM();
        contribution getECMContributions();

        std::vector<double> getECM(bool data=false);
        std::vector<double> getECM_boundary(bool data=false);
        std::vector<double> getECM_prefetch(bool data=false);
        std::vector<double> getECM_assoc(bool data=false);
        std::vector<double> getECM_latency();

        std::vector<double> getECM_validate(bool data=false);

        double getPerfECM();
        double getECM_BurstPenalty();
        std::vector<double> getSaturation();
        double getCpuFreq();

        void write2dFile(const char *filename, char* grid_name, char* dim_str, bool halo);
};

struct Grid
{
    yaskSite* stencil;
    int currTime;
    GRID grid_details; //offline grid

    Grid(yaskSite* stencil_, GRID grid_details_):stencil(stencil_),currTime(0),grid_details(grid_details_)
    {
    }

    Grid(const Grid& other):stencil(other.stencil),currTime(other.currTime),grid_details(other.grid_details)
    {
    }

    inline Grid& operator=(Grid& other)
    {
        stencil = other.stencil;
        currTime = other.currTime;
        grid_details = other.grid_details;
        return *this;
    }

    /*can't manipulate time the current API
     * inline Grid operator[] (int ts)
    {
        currTime = ts;
        return *this;
    }*/
};

inline bool operator<<(Grid lhs, Grid rhs)
{
   /*const char* lhs_grid_name = lhs.grid_details.name.c_str();
    const char* rhs_grid_name = rhs.grid_details.name.c_str();
    /printf("fusing %s and %s\n", lhs_grid_name, rhs_grid_name);
    rhs.stencil->setElement(0.123, rhs_grid_name, 0, 10, 10, 10);
    rhs.stencil->setElement(0.321, rhs_grid_name, 1, 10, 10, 10);
    printf("I am setting on RHS at (10,10,10) = 0.123\n");
    printf("before fusion %p and %p\n", lhs.stencil->getGridPtr(lhs_grid_name), rhs.stencil->getGridPtr(rhs_grid_name));
    printf("val at lhs(%d,10,10,10) and rhs(%d,10,10,10) = %f and %f\n", 0, 0, lhs.stencil->getElement(lhs_grid_name, 0, 10, 10, 10), rhs.stencil->getElement(rhs_grid_name, 0, 10, 10, 10));
    */
    lhs.stencil->fuseGrid(lhs.grid_details.name.c_str(), rhs.stencil, rhs.grid_details.name.c_str());
    /*printf("after fusion %p and %p\n", lhs.stencil->getGridPtr(lhs_grid_name), rhs.stencil->getGridPtr(rhs_grid_name));
    printf("val at lhs(%d,10,10,10) and rhs(%d,10,10,10) = %f and %f\n", lhs.stencil->totalTime, rhs.stencil->totalTime, lhs.stencil->getElement(lhs_grid_name, lhs.stencil->totalTime, 10, 10, 10), rhs.stencil->getElement(rhs_grid_name, rhs.stencil->totalTime, 10, 10, 10));
    */

    return true;
}


#endif
