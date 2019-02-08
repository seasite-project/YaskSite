#ifndef YASKSITE_TYPES_H
#define YASKSITE_TYPES_H

#include "config.h"
#include <vector>
#include <string>

class yaskSite;

struct GRID
{
    int idx;
    std::string name;
    int dim;
    std::vector<int> halo;
    int time_steps;
    GRID(int idx_, std::string name_, int dim_);
    int size(int dt, int dx, int dy, int dz);
};

class perfModel;

struct EQ_GROUP
{
    std::string name;
    int num_reads;
    int num_writes;

    //constant params are avoided; only the once
    //with spatial dimension are included
    int num_spatial_reads;
    int num_spatial_writes;

    std::vector<GRID> read_grids;
    std::vector<GRID> write_grids;

    //finalize the eqn. group by calculating num_spatial_reads/writes
    //and checks for dimensional consistency
    bool finalize();

    //points in bounding box for this eqGroup
    int num_points;

    perfModel* model;
};


struct STENCIL
{
    char* name;
    int dim;
    int radius;
    int s;
    int fold_x;
    int fold_y;
    int fold_z;
    bool prefetch;
    char* path;
    char* folder; //folder in which the stencil is cached
    yaskSite* data;
    STENCIL();
    bool checkMinDef();
};

struct cache_info
{
    double bytes;
    double words;
    std::string name;
    std::string details;
    bool shared;
    int hierarchy;
    int set_size;
    int ways;
    std::vector<double> latency; //latency in ns as a function of threads
    double sf;
    int prefetch_cl;
    int bytePerWord;
    double getLatency(int threads);
    void readLatency(char* file);
    cache_info(char* name_, char* details_, int hierarchy_, int set_size=-1, int ways=-1, std::vector<double> latency_={}, int prefetch_cl_=0, double sf_=1, int bytePerWord=8);
//    cache_info();
};

//L1-cache given a safety factor since they have to store constants and program
//specific arrays; TODO need to find a way to determine this sf factor by
//benchmark; The extra CL prefetched is really accurate; it can be found by
//benchmarks while scanning sizes from 110-140, with and without h/w prefetching

//static std::vector<cache_info> CACHES{(cache_info(L1_cache,1,{1.83},1,0.5)), (cache_info(L2_cache,2,{5.44},20,0.5)), (cache_info(L3_cache,3,{21},20,0.5)), (cache_info(MEM,4,{84,84.27,87.57,92.21,97.07,101.90,107.08,112.50,118.95,124.34}))};


//IVB
static std::vector<cache_info> CACHES{(cache_info("L1",L1_cache,0,64,8,{},0,1.5)),(cache_info("L2",L2_cache,1,512,8,{},2.215,1.5)),(cache_info("L3",L3_cache,2,25600,16,{},7.61,1.5)), (cache_info("MEM",MEM,3,-1,-1,{},18.95))};
//BDW
//static std::vector<cache_info> CACHES{(cache_info("L1",L1_cache,0,64,8,{},0,1.5)),(cache_info("L2",L2_cache,1,512,8,{},2.171096,1.5)),(cache_info("L3",L3_cache,2,5632,64,{},11.4325,1.5)), (cache_info("MEM",MEM,3,-1,-1,{},18.9327))};

//static std::vector<cache_info> CACHES{(cache_info(L1_cache,1,{0},1,0.5)), (cache_info(L2_cache,2,{0},20,0.5)), (cache_info(L3_cache,3,{0},20,0.5)), (cache_info(MEM,4,{0}))};

//static std::vector<cache_info> CACHES{(cache_info(L1_cache,1,{0},0,0.5)), (cache_info(L2_cache,2,{0},0,0.5)), (cache_info(L3_cache,3,{0},0,0.5)), (cache_info(MEM,4,{0}))};


cache_info CACHE(char* str);

enum LC {OUTER, INNER};
enum DIM {x, y, z};


struct contribution
{
    double latency;
    double boundary;
    double prefetch;
    double associativity;
};


#endif
