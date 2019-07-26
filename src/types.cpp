#include <stdlib.h>
#include "types.h"
#include "print.h"
#include <iostream>
#include <array>
#include "macros.h"
#include "config.h"

std::vector<cache_info> CACHES;

GRID::GRID(int idx_, std::string name_, int dim_):idx(idx_),name(name_), dim(dim_)
{
}

int GRID::size(int dt, int dx, int dy, int dz)
{
    std::array<int,4> dim_sizes = {dt,dx,dy,dz};

    int out=1;
    for(int d=0; d<dim; ++d)
    {
        out *= dim_sizes[d];
    }

    return out;
}

bool EQ_GROUP::finalize()
{
    bool ret =true;

    num_spatial_reads=0;
    for(int i=0; i<(int)read_grids.size(); ++i)
    {
        int curr_dim = read_grids[i].dim;
        if( ( (curr_dim != 0)&&(curr_dim!=3) ) && (curr_dim!=4) )
        {
            ERROR_PRINT("Grid Dimension not compatible check : eq. Group %s", name.c_str());
            /*ret = false;
            break;*/
        }
        if( (curr_dim==3) || (curr_dim==4) )
        {
            ++num_spatial_reads;
        }
    }

    num_spatial_writes=0;
    for(int i=0; i<(int)write_grids.size(); ++i)
    {
        int curr_dim = write_grids[i].dim;
        if( ( (curr_dim != 0)&&(curr_dim!=3) ) && (curr_dim!=4) )
        {
            ERROR_PRINT("Grid Dimension not compatible check : eq. Group %s", name.c_str());
            /*ret = false;
            break;*/
        }
        if( (curr_dim==3) || (curr_dim==4) )
        {
            ++num_spatial_writes;
        }
    }

    //printf("spatial: read = %d, write = %d\n", num_spatial_reads, num_spatial_writes);
    return ret;
}

STENCIL::STENCIL():name(NULL),dim(0),radius(-1),fold_x(1),fold_y(1),fold_z(1),prefetch(false),path(NULL),folder(NULL),data(NULL)
{
}

bool STENCIL::checkMinDef()
{
    bool ret = true;

    if(name==NULL)
        return false;

    if(dim!=2 || dim!=3)
    {
        if(dim==0)
        {
            ERROR_PRINT("Please set the dimension of the stencil.");
        }
        else
        {
            ERROR_PRINT("Given dimension %d not supported, Currently yaskSite supports only dimension 2 and 3", dim);
        }
        ret = false;
    }

    //TODO check if they are of vector size
    if( (fold_x < 1 || fold_y < 1) || fold_z < 1)
    {
        ERROR_PRINT("Please recheck the given fold size '%d x %d x %d'", fold_x, fold_y, fold_z);
        ret = false;
    }

    if( ! ( ((strcmp(path,"serpentine")==0)||(strcmp(path,"default")==0)) || (strcmp(path,"")==0)) )
    {
        ERROR_PRINT("Unsupported path; path should be either 'default' or 'serpentine'");
        ret = false;
    }

    return ret;
}
/*
cache_info::cache_info(char* name_, char* details_, int hierarchy_, int set_size_, int ways_, std::vector<double> latency_, int prefetch_cl_, double sf_, int bytePerWord_):name(name_),details(details_),set_size(set_size_),ways(ways_),latency(latency_),prefetch_cl(prefetch_cl_),bytePerWord(bytePerWord_)
{
    //writable string
    char* cache_str_w = strdup(details_);
    char* byte_str = strtok(cache_str_w,";");
    char* shared_str = strtok(NULL,";");
    bytes=atof(byte_str);
    shared=(strstr(shared_str, "shared")!=NULL)?true:false;
    words=bytes/bytePerWord;
    free(cache_str_w);
    hierarchy = hierarchy_;
    sf = sf_;

    char* lat_file;
    STRINGIFY(lat_file, "%s/bench_results/latency/%s.txt", TEMP_DIR, name.c_str());
    readLatency(lat_file);
    free(lat_file);

    if(set_size < 0)
    {
        set_size = static_cast<int>(bytes/(64.0)); //assuming 64 byte wide cache line
        ways = 1;
    }
}
*/
cache_info::cache_info(int cacheId, char *mc_file, int isMEM_): hierarchy(cacheId), isMEM(isMEM_)
{
    char* sysLogFileName = NULL;
    FILE *tmp;
    POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d name", TOOL_DIR, mc_file, cacheId);
    char *cacheName = readStrVar(tmp);
    PCLOSE(tmp);
    name=cacheName;
    free(cacheName);

    bytePerWord = sizeof(double);
    sf = 0.9;

    printf("MEM = %d\n", isMEM);
    if(!isMEM)
    {
        POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d ways", TOOL_DIR, mc_file, cacheId);
        int cacheWays = readIntVar(tmp);
        PCLOSE(tmp);
        ways=cacheWays;

        POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d sets", TOOL_DIR, mc_file, cacheId);
        int cacheSets = readIntVar(tmp);
        PCLOSE(tmp);
        set_size=cacheSets;
        printf("set size = %d\n", set_size);
        POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d cl_size", TOOL_DIR, mc_file, cacheId);
        int cache_cl_size = readIntVar(tmp);
        PCLOSE(tmp);
        cl_size = cache_cl_size;

        bytes = (double)ways*(double)set_size*cl_size;
    }
    else
    {
        POPEN(sysLogFileName, tmp, "%s/cacheInfo/getMemInfo.sh %s", TOOL_DIR, mc_file);
        double size = readDoubleVar(tmp);
        ways=-1;
        set_size=-1;
        bytes = size;
        PCLOSE(tmp);

        POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d cl_size", TOOL_DIR, mc_file, cacheId-1);
        int cache_cl_size = readIntVar(tmp);
        PCLOSE(tmp);
        cl_size = cache_cl_size;
    }

    words = bytes/bytePerWord;

    printf("%s words = %f\n", name.c_str(), words);
    POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d cores", TOOL_DIR, mc_file, cacheId);
    int cacheCores = readIntVar(tmp);
    PCLOSE(tmp);
    cores=cacheCores;
    if(cores > 1)
    {
        shared=true;
    }

    char* lat_file;
    STRINGIFY(lat_file, "%s/bench_results/latency/%d", TEMP_DIR, cacheId);
    readLatency(mc_file);//lat_file);
    free(lat_file);

    POPEN(sysLogFileName, tmp, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;prefetch distance\"", TOOL_DIR, mc_file, name.c_str());
    prefetch_cl = readDoubleVar(tmp);
    printf("Prefetch dist = %f\n", prefetch_cl);
    PCLOSE(tmp);
}

double cache_info::getLatency(int nthreads)
{
    if(latency.empty())
    {
        return 0;
    }
    else
    {
        if(latency.size()==1)
        {
            //Increasing thread should not change anything
            //for eg. in caches
            return latency[0];
        }
        else
        {
            return latency[nthreads-1];
        }
    }
}

void cache_info::readLatency(char* mc_file)
{
    bool success = true;
    FILE *file;
    char *sysLogFileName = NULL;
    POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;results;latency\" | sed -e \"s@cy@@g\" | sed -e \"s@\\[@@g\" | sed -e \"s@\\]@@g\"", TOOL_DIR, mc_file, name.c_str());
    char* latency_str = readStrVar(file);
    //success = readTable(file, latency, 1, cores, 1, 1);
    latency=split_double(latency_str, ',');
    PCLOSE(file);

    if(!success)
    {
        printf("Latency measurements not done, please go to build directory of YASKSITE and execute 'make calibrate'\n");
    }
}

/*cache_info::cache_info()
{
}*/

/*
cache_info::~cache_info()
{
    if(name)
        free(name);
}*/

//Parsing from yaml machine file
void initializeCaches()
{
    char *mc_file_loc;
    STRINGIFY(mc_file_loc, "%s/mc_file.txt", TOOL_DIR);

    FILE *file = fopen(mc_file_loc, "r");
    char *mc_file = readStrVar(file);
    fclose(file);
    free(mc_file_loc);

    char *sysLogFileName = NULL;
    FILE *tmp;
    POPEN(sysLogFileName, tmp, "%s/cacheInfo/numCache.sh %s", TOOL_DIR, mc_file);
    int numCaches = readIntVar(tmp);
    PCLOSE(tmp);

    for(int cacheId=0; cacheId<numCaches; ++cacheId)
    {
        CACHES.push_back(cache_info(cacheId, mc_file));
        printf("Initialized %s cache\n", CACHES[cacheId].name.c_str());
    }

    CACHES.push_back(cache_info(numCaches, mc_file, true));
    free(mc_file);

    printf("prefetch dist L1 = %f\n", CACHES[0].prefetch_cl);
    printf("prefetch dist MEM = %f\n", CACHES[3].prefetch_cl);
}

cache_info CACHE(char* str)
{
    if(strcmp(str,"L1")==0)
    {
        return CACHES[0];
    }
    else if(strcmp(str,"L2")==0)
    {
        return CACHES[1];
    }
    else if(strcmp(str,"L3")==0)
    {
        return CACHES[2];
    }
    else if(strcmp(str,"MEM")==0)
    {
        return CACHES[3];
    }
    else
    {
        ERROR_PRINT("Unknown Cache hierarchy");
        return CACHES[3];
    }
}
