#include <stdlib.h>
#include "types.h"
#include "print.h"
#include <iostream>
#include <array>
#include "macros.h"
#include "config.h"

std::vector<cache_info> CACHES;
char* glb_mc_file = NULL;

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
            ERROR_PRINT("Grid Dimension not compatible check, dimension = %d : eq. Group %s", curr_dim, name.c_str());
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

STENCIL::STENCIL():name(NULL),dim(0),radius(-1),fold_x(1),fold_y(1),fold_z(1),dp(true),prefetch(false),path(NULL),folder(NULL),data(NULL)
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
    stencilInit = false;
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

    //words = bytes/bytePerWord;
    //printf("%s words = %f\n", name.c_str(), words);
    POPEN(sysLogFileName, tmp, "%s/cacheInfo/getCacheInfo.sh %s %d cores", TOOL_DIR, mc_file, cacheId);
    int cacheCores = readIntVar(tmp);
    PCLOSE(tmp);
    cores=cacheCores;
    if(cores > 1)
    {
        shared = true;
    }
    else
    {
        shared = false;
    }

    penalty = 0;
    POPEN(sysLogFileName, tmp, "%s/yamlParser/yamlParser %s \"memory hierarchy;%d;penalty cycles per mem cy\"", TOOL_DIR, mc_file, hierarchy);
    penalty = readDoubleVar(tmp);
    printf("%s penalty = %f\n", name.c_str(), penalty);
    PCLOSE(tmp);

    victim = false;
    //char* lat_file;
    //STRINGIFY(lat_file, "%s/bench_results/latency/%d", TEMP_DIR, cacheId);
    readLatency(mc_file);//lat_file);
    //free(lat_file);
    //
    readBytePerCycle(mc_file);

    POPEN(sysLogFileName, tmp, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;prefetch distance\"", TOOL_DIR, mc_file, name.c_str());
    prefetch_cl = readDoubleVar(tmp);
    printf("Prefetch dist = %f\n", prefetch_cl);
    PCLOSE(tmp);
}

double cache_info::getWords()
{
    if(!stencilInit)
    {
        WARNING_PRINT("Stencil not intialized: bytePerWord cannot be determined");
    }
    return bytes/(double)bytePerWord;
}

double cache_info::getBytePerCycle(int rwRatio, int nthreads)
{
    if(bytePerCycle.empty())
    {
        return 0;
    }
    else
    {
        int numRW = (int)bytePerCycle.size();
        int numThreads = (int)bytePerCycle[0].size();

        if(rwRatio > numRW)
        {
            rwRatio = numRW;
        }
        if(rwRatio < 1)
        {
            rwRatio = 1;
            //ERROR_PRINT("Wrong rw ratio");
        }

        if(nthreads > numThreads)
        {
            nthreads = numThreads;
        }
        if(nthreads < 1)
        {
            ERROR_PRINT("Could not retrieve bandwidth informations\n");
        }
        return bytePerCycle[rwRatio-1][nthreads-1];
    }
}


double cache_info::getLatency(int rwRatio, int nthreads)
{
    if(latency.empty())
    {
        return 0;
    }
    else
    {
        int numRW = (int)latency.size();
        if(rwRatio > numRW)
        {
            rwRatio = numRW;
        }
        if(rwRatio < 1)
        {
            rwRatio = 1;
            //ERROR_PRINT("Wrong rw ratio");
        }
        return latency[rwRatio-1][nthreads-1];
    }
}

void cache_info::readBytePerCycle(char* mc_file)
{
    FILE *file;
    char *sysLogFileName = NULL;

    bool success = false;

    if(hierarchy == 0)
    {
        success = true;
    }
    else if(isMEM)
    {
        POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;MEM;1;LDST;BW\"| sed -e \"s@GB/s@@g\" | sed -e \"s@\\[@@g\" | sed -e \"s@\\]@@g\"", TOOL_DIR, mc_file);
        char *bw_str = readStrVar(file);
        std::vector<double> bw_table = split_double(bw_str, ',');
        PCLOSE(file);
        if(!std::string(bw_str).empty())
        {
            success=true;
        }

        POPEN(sysLogFileName, file, "%s/getFreq.sh %s", TOOL_DIR, mc_file);
        double cpu_freq = readDoubleVar(file);
        PCLOSE(file);

        printf("MEM values\n");
        for(unsigned i=0; i<bw_table.size(); ++i)
        {
            printf("%f\n",bw_table[i]/(cpu_freq));
            bytePerCycle.push_back({bw_table[i]/(cpu_freq)});
        }

        free(bw_str);
    }
    else
    {
        if(!shared)
        {

            POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"memory hierarchy;%d;upstream throughput;0\" |  grep -Eo '[+-]?[0-9]+([.][0-9]+)?'", TOOL_DIR, mc_file, hierarchy);
            double value = readDoubleVar(file);

            if(value>0)
            {
                success = true;
            }

            printf("%s hierarchy = %d values\n %f\n", name.c_str(),hierarchy, value);
            bytePerCycle.push_back({value});
            PCLOSE(file);

            POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"memory hierarchy;%d;upstream throughput;1\"", TOOL_DIR, mc_file, hierarchy);
            std::string duplexity(readStrVar(file));
            printf("check duplexity = %s\n", duplexity.c_str());
            if(duplexity.find("half-duplex") == std::string::npos)
            {
                ERROR_PRINT("Currently %s cache with only half-duplexity treated, setting the cache as half-duplex", name.c_str());
            }
            PCLOSE(file);
        }
        else
        {
            POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;results;data path bw\" | wc -l", TOOL_DIR, mc_file, name.c_str());
            int numRW = readIntVar(file);
            PCLOSE(file);
            printf("Num rw = %d\n", numRW);
            printf("%s values\n", name.c_str());
            for(int i=1; i<=numRW; ++i)
            {
                success = true;
                POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;results;data path bw;%d\" | sed -e \"s@B/cy@@g\" | sed -e \"s@\\[@@g\" | sed -e \"s@\\]@@g\"", TOOL_DIR, mc_file, name.c_str(), i);
                char* bw_str = readStrVar(file);
                bytePerCycle.push_back(split_double(bw_str, ','));
                for(int j=0; j<(bytePerCycle.back()).size(); ++j)
                {
                    printf("%f \t", bytePerCycle.back()[j]);
                }
                printf("\n");
                PCLOSE(file);
                free(bw_str);
            }
        }
    }
    if(!success)
    {
        ERROR_PRINT("Missing bandwidth informations, please go to build directory of YASKSITE and execute 'make calibrate', and make sure cache upstream throughputs are entered");

    }
}

void cache_info::readLatency(char* mc_file)
{
    FILE *file;
    char *sysLogFileName = NULL;
    POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;results;latency\" | wc -l", TOOL_DIR, mc_file, name.c_str());
    int numRW = readIntVar(file);
    PCLOSE(file);

    for(int i=1; i<=numRW; ++i)
    {
        bool success = true;
        POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;%s;1;results;latency;%d\" | sed -e \"s@ns@@g\" | sed -e \"s@\\[@@g\" | sed -e \"s@\\]@@g\"", TOOL_DIR, mc_file, name.c_str(), i);
        char* latency_str = readStrVar(file);
        //success = readTable(file, latency, 1, cores, 1, 1);
        latency.push_back(split_double(latency_str, ','));

        PCLOSE(file);

        if(!success)
        {
            printf("Latency measurements not done, please go to build directory of YASKSITE and execute 'make calibrate'\n");

        }
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
void initializeCaches(char *mcFile_user)
{
    char *mc_file = NULL;
    if(mcFile_user == NULL)
    {
        char *mc_file_loc;
        STRINGIFY(mc_file_loc, "%s/mc_file.txt", TOOL_DIR);

        FILE *file = fopen(mc_file_loc, "r");
        mc_file = readStrVar(file);
        fclose(file);
        free(mc_file_loc);
    }
    else
    {
        mc_file = mcFile_user;
    }

    glb_mc_file = mc_file;

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

    for(int cacheId=0; cacheId<numCaches; ++cacheId)
    {
        //check for any victim caches
        POPEN(sysLogFileName, tmp, "%s/yamlParser/yamlParser %s \"memory hierarchy;%d;cache per group;victims_to\"", TOOL_DIR, mc_file, cacheId);
        char* vict_cache = readStrVar(tmp);
        if(strcmp(vict_cache,"L1")==0)
        {
            CACHES[0].victim=true;
        }
        else if(strcmp(vict_cache,"L2")==0)
        {
            CACHES[1].victim=true;
        }
        else if(strcmp(vict_cache,"L3")==0)
        {
            CACHES[2].victim=true;
        }

        PCLOSE(tmp);
    }

#if 1
    //remove latency of previous cache from current one
    for(int cacheId=numCaches; cacheId>=1; --cacheId)
    {
        cache_info* currCache = &(CACHES[cacheId]);
        cache_info* prevCache = &(CACHES[cacheId-1]);

/*        int ctr=1;
        //because the currCache is loaded to prev-prev
        while(prevCache->victim && ((cacheId-1-ctr)>=0))
        {
            prevCache = &(CACHES[cacheId-1-ctr]);
            ++ctr;
        }
*/
        for(int rw=0; rw<(int)(currCache->latency.size()); ++rw)
        {
            for(int thread=0; thread<(int)(currCache->latency[rw].size()); ++thread)
            {
                if(currCache->latency[rw][thread] > prevCache->latency[rw][thread])
                {
                    currCache->latency[rw][thread] =  currCache->latency[rw][thread] - prevCache->latency[rw][thread];
                }
            }
        }
    }
#endif

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

void updateBytePerWord(int bytePerWord_)
{
    for(int i=0; i<(int)CACHES.size(); ++i)
    {
        CACHES[i].bytePerWord = bytePerWord_;
        CACHES[i].stencilInit = true;
    }
}
