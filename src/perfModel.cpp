#include "perfModel.h"
#include "config.h"
#include "macros.h"
#include "print.h"
#include "types.h"
#include "yaskSite.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <likwid.h>
//#include <cmath>
#include <algorithm>
#include <list>
#include <float.h>

//TODO:get cache hierarchies from hwloc
perfModel::perfModel(STENCIL* stencil_, double cpu_freq_, char* iacaOut):stencil(stencil_), cpu_freq(cpu_freq_), likwidInited(false), cpus(NULL), ECM_latency(4,0), ECM_prefetch(4,0), ECM_boundary(4,0), ECM_assoc(4,0), ECM_penalty(0), ECM_extra_work_percent(0)
{
    //allow for stencil=NULL; reqd. for derived stencil
    if(stencil)
    {
        //generate IACA
        /*char* iacaOut;
          STRINGIFY(iacaOut, "%s/iacaFolder", stencil->folder);
          SYSTEM(NULL,"%s/generateIACA.sh %s \"%s\" %s %s", TOOL_DIR, stencil->folder, IACA_PATH, "IVB", iacaOut);*/

        //give a value assuming only one write and max grids in the stencil
        //assign more accurate value once stencil init() is called and bounding
        //boxes known
        numReadGrids = stencil->s - 1;
        numWriteGrids = 1;
        //numStencils = 1;

        //default weight=1
        weight = 1;
        derived = false; //pure kernels by default
        val = false;
        ECM_penalty = 0;

        if(iacaOut!=NULL)
        {
            //get port cycles and throughput
            parseIACA(iacaOut);
        }
        else
        {
            setDerived(true);
        }

        //getBytePerCycles();
    }
}

perfModel::~perfModel()
{
    if(likwidInited && cpus != NULL)
    {
        free(cpus);
        perfmon_finalize();
        affinity_finalize();
        topology_finalize();
    }
}

void perfModel::setReadWriteGrids(int numReadGrids_, int numWriteGrids_, int numStencils_)
{
    numReadGrids = numReadGrids_;
    numWriteGrids = numWriteGrids_;
    numStencils = numStencils_;
}

void perfModel::setWeight(double weight_)
{
    weight = weight_;
}

void perfModel::setDerived(bool derived_)
{
    derived = derived_;
}

std::vector<std::string> readIACAout(char* iacaFile, char* entity)
{
    std::vector<std::string> str_vec;

    FILE* fileStream;
    //open port details
    fileStream = fopen(iacaFile, "r");
    //Read port names into map
    char *str = (char*) malloc(sizeof(char)*256);
    fscanf(fileStream, "%s", str);
    if(strcmp(str,  entity) != 0)
    {
        ERROR_PRINT("Seems like an error occured in IACA parsing\n");
    }

    int ret = fscanf(fileStream, "%s", str);
    char *prev=NULL;
    bool combine = false;

    while(ret != EOF)
    {
        if(combine)
        {
            char* newStr;
            STRINGIFY(newStr, "%s:%s", prev, str);
            free(str);
            str = newStr;
            combine=false;
        }
        str_vec.push_back(str);

        if(prev)
            free(prev);

        prev = strdup(str);
        ret = fscanf(fileStream, "%s", str);
        if((ret != EOF) && (strcmp(str, "-")==0))
        {
            combine=true;
            ret = fscanf(fileStream, "%s", str);
        }
    }
    free(prev);
    free(str);

    fclose(fileStream);
    return str_vec;
}


void perfModel::parseIACA(char* iacaFolder)
{
    char *portFile, *cycleFile, *btFile;
    STRINGIFY(portFile, "%s/iaca_port.txt", iacaFolder);
    STRINGIFY(cycleFile, "%s/iaca_cycle.txt", iacaFolder);
    STRINGIFY(btFile, "%s/iaca_blockThroughput.txt", iacaFolder);

    std::vector<std::string> port = readIACAout(portFile, "Port");
    std::vector<std::string> cycles = readIACAout(cycleFile, "Cycles");
    //get block throughput
    FILE *fileStream = fopen(btFile,"r");
    blockThroughput = readIntVar(fileStream);
    fclose(fileStream);

    if(port.size() != cycles.size())
    {
        ERROR_PRINT("Error in IACA parsing ports and cycles do not match");
    }

    int len = (int)port.size();
    //make a map of them
    for(int i=0; i<len; ++i)
    {
        portCycle[port[i]] = std::stof(cycles[i]);
    }

    //get LD ports
    char* LD_PORTS_dup = strdup(LD_PORTS);
    char* ld_ports_str = strtok(LD_PORTS_dup, ";");
    do {
        LD_ports.push_back(ld_ports_str);
    } while(strtok(NULL, ";"));

    free(LD_PORTS_dup);
}

#if 0
void perfModel::getBytePerCycles()
{

    char *mc_file_loc;
    STRINGIFY(mc_file_loc, "%s/mc_file.txt", TOOL_DIR);

    FILE *file = fopen(mc_file_loc, "r");
    char *mc_file = readStrVar(file);
    fclose(file);
    free(mc_file_loc);


    if(strcmp(TARGET_ARCH,"ivb")==0 || strcmp(TARGET_ARCH,"snb")==0)
    {
        bytePerCycle["L2"] = {32};
        bytePerCycle["L3"] = {32};
    }
    else if(strcmp(TARGET_ARCH,"hsw")==0)
    {
        bytePerCycle["L2"] = {32};
        bytePerCycle["L3"] = {32};
    }
    else if(strcmp(TARGET_ARCH, "skx")==0)
    {
        bytePerCycle["L2"] = {64};
        bytePerCycle["L3"] = {12};//{32*0.7};
    }
    else if(strcmp(TARGET_ARCH, "knl")==0)
    {
        bytePerCycle["L2"] = {48};
        bytePerCycle["L3"] = {1000000000};//no L3
    }
    else
    {
        ERROR_PRINT("Dont't have sufficient data to do a perf. model");
    }


    std::vector<double> bw_table;

    /*char* bw_file;
      STRINGIFY(bw_file, "%s/bench_results/bw_load_ratio.txt", TEMP_DIR);
      bool success = readTable(bw_file, bw_table, 1, 10, 2, 2);
      if(success==false)
      {
      ERROR_PRINT("Memory bandwidth not calibrated, please go to build directory of YASKSITE and execute make calibrate");
      }
      delete[] bw_file;
      */

    char *sysLogFileName = NULL;
    POPEN(sysLogFileName, file, "%s/yamlParser/yamlParser %s \"benchmarks;measurements;MEM;1;LDST;BW\"| sed -e \"s@GB/s@@g\" | sed -e \"s@\\[@@g\" | sed -e \"s@\\]@@g\"", TOOL_DIR, mc_file);
    char *bw_str = readStrVar(file);
    bw_table = split_double(bw_str, ',');
    PCLOSE(file);

    free(mc_file);

    for(unsigned i=0; i<bw_table.size(); ++i)
    {
        bw_table[i]=bw_table[i]/(cpu_freq);
    }
    bytePerCycle["MEM"] = bw_table;//18.5;
}
#endif

//Values are in words
//temporalStoreMode - 0 - read and write (default)
//                    1 - no store
//                    2 - store only
//Return {Total Data, LD:ST ratio}
std::vector<double> perfModel::getDataContrib(char* cache_str, blockDetails* opt, int temporalStoreMode)
{
    cache_info currCache = CACHE(cache_str);

    int prevHierarchy;
    if((currCache.hierarchy-1) >= 0)
    {
        prevHierarchy = currCache.hierarchy-1;
    }
    else
    {
        prevHierarchy = currCache.hierarchy;
    }
    cache_info prevCache = CACHES[prevHierarchy];

    int victim_scale = 1;
    if(currCache.victim)
    {
        victim_scale = 2;
    }

    //Right now assuming only one stencil per eqn. Group
    double rest_contrib = (numReadGrids-numStencils)+2*numWriteGrids;//1.0+(stencil->s-1.0);
    if(currCache.victim)
    {
        rest_contrib = (numReadGrids-numStencils)+numWriteGrids;
    }
    printf("Read Grids = %d, writeGrids = %d\n", numReadGrids, numWriteGrids);
    int totalGrids = numReadGrids+numWriteGrids;

    double prefetch_oh_per_LUP = getPrefetchEffects(currCache, opt);

    spatialOh = addBlockBoundaryEffects(currCache);
    temporalOh = addBlockBoundaryEffects(currCache,true);

    //if it's greater than temporal blocked cache;
    //I put 0 contribution from this cache/MEM
    if( currCache.hierarchy > opt->fit_domain->hierarchy)
    {
        return {0.0,0.0};
    }
    else if(currCache.hierarchy > opt->temporal->hierarchy)
    {
        double extra_data = spatialOh[0]+totalGrids*prefetch_oh_per_LUP;
        double shrink_space = 1.0 - (extra_data)/(extra_data+rest_contrib+1.0);
        bool ilc_jump = false, olc_jump = false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);

        double totalSpatialContrib = (rest_contrib+numStencils*1.0+extra_data+assocOh[0]);/*TODO compute per region*/
        double totalStreams = (totalGrids);

        if(temporalStoreMode == 1)
        {
            totalSpatialContrib-=numWriteGrids; //NO WA
        }
        else if(temporalStoreMode == 2)
        {
            totalSpatialContrib=numWriteGrids; //only store
        }

        //printf("##### Temporal pref of %s = %f\n", currCache.name.c_str(), prefetch_oh_per_LUP);
        //temporal effect only from second timestep; i.e. when the dx reduces
        //with prescribed angle

        //write grids
        ECM_boundary[currCache.hierarchy] = victim_scale*((1+rest_contrib)*temporalOh[0]*currCache.bytePerWord + spatialOh[0]*currCache.bytePerWord/static_cast<double>(stencil->data->dt));
        ECM_prefetch[currCache.hierarchy] = victim_scale*(prefetch_oh_per_LUP*totalStreams*currCache.bytePerWord/static_cast<double>(stencil->data->dt));
        ECM_assoc[currCache.hierarchy] = assocOh[0]*currCache.bytePerWord/static_cast<double>(stencil->data->dt);

        int rwRatio = static_cast<int>((totalStreams-numWriteGrids)/(double)numWriteGrids);
        double latency_oh_per_LUP_temp = getLatencyEffects(currCache, rwRatio, true)*(stencil->data->dt-1.0)/((double)stencil->data->dt);
        //if prevCache is victim, then we have to add extra latency btw L3 and
        //L2 as this is not accounted by MEM latency measurement. This is the
        //only case where you need it, in another cases since smaller memory
        //hierarchy satisfy only lesser LC, we do not need to add, but remember 
        //since L3 cache is busy by all threads latency with all threads have to 
        //be accounted
        //But this is only required if LLC prefetcher is off
        /*        int  prevId = currCache.hierarchy-1;

                  while(prevId>=0)
                  {
                  cache_info prev_cache = CACHES[prevHierarchy];
                  double prev_latency_oh_per_LUP_temp = getLatencyEffects(prev_cache, rwRatio, true, 1)*(stencil->data->dt-1.0)/((double)stencil->data->dt);
                  ECM_latency[prev_cache.hierarchy] += prev_latency_oh_per_LUP_temp;
                  --prevId;
                  }*/
        double latency_oh_per_LUP = getLatencyEffects(currCache, rwRatio);
        ECM_latency[currCache.hierarchy] = latency_oh_per_LUP_temp + latency_oh_per_LUP/static_cast<double>(stencil->data->dt);
        return { victim_scale*(((totalSpatialContrib)/static_cast<double>(stencil->data->dt))+(1+rest_contrib)*temporalOh[0]), ((totalSpatialContrib-numWriteGrids)/(double)numWriteGrids) };
    }
    //if greater than Outer blocked LC
    else if(currCache.hierarchy > opt->spatialOBC->hierarchy)
    {
        double prefetch_oh = totalGrids*prefetch_oh_per_LUP;
        double boundary_oh = spatialOh[0];

        double bx = stencil->data->bx;
        double by = stencil->data->by;
        double bz = stencil->data->bz;
        double totThreads = stencil->data->nthreads;
        if(currCache.shared && (totalGrids+numStencils)*bz*by*bx*totThreads > currCache.getWords())
        {
            boundary_oh = spatialOh[1];
        }
        double extra_data = boundary_oh+prefetch_oh;
        double actual_data = rest_contrib+1.0*numStencils;
        double shrink_space = 1.0 - (extra_data)/(extra_data+actual_data);
        bool ilc_jump = false, olc_jump = false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);
        //double latency_oh = (totalGrids)*latency_oh_per_LUP;
        int dim = stencil->dim;
        bool jump = (dim>2)?olc_jump:ilc_jump;
        int rwRatio = static_cast<int>((totalGrids - numWriteGrids)/(double)(numWriteGrids));
        if(jump)
        {
            double outer_fold=1;
            if(dim==3)
            {
                outer_fold=stencil->fold_x;
            }
            else
            {
                outer_fold=stencil->fold_y;
            }
            int stencil_contrib_outer = 2*( static_cast<int>((stencil->radius-1)/(outer_fold)) + 1 );
            int stencil_centre = 1;
            double streams = (numStencils*(stencil_contrib_outer + stencil_centre) + totalGrids - numStencils);
            prefetch_oh = streams*prefetch_oh_per_LUP;
            boundary_oh = spatialOh[2];
            extra_data = prefetch_oh + boundary_oh;
            //latency_oh = streams*latency_oh_per_LUP;
            rwRatio = static_cast<int>((streams - numWriteGrids)/(double)numWriteGrids);
        }

        double assoc_oh = (dim>2)?assocOh[0]:assocOh[1];
        double totalData = actual_data + extra_data + assoc_oh;
        ECM_prefetch[currCache.hierarchy] = victim_scale*prefetch_oh*currCache.bytePerWord;
        ECM_boundary[currCache.hierarchy] = victim_scale*boundary_oh*currCache.bytePerWord;
        ECM_assoc[currCache.hierarchy] = assoc_oh*currCache.bytePerWord;

        //        printf("....total Grids = %d, numWriteGrids = %d\n", totalGrids, numWriteGrids);
        double latency_oh = getLatencyEffects(currCache, rwRatio);
        ECM_latency[currCache.hierarchy] = latency_oh;
        return { victim_scale*totalData, (totalData-numWriteGrids)/((double)numWriteGrids) };
    }
    //TODO: only for symmetric stencils currently
    //if greater than Inner blocked LC
    //stencil_data = (2*(int((rad-1)/fold) + 1) + 1)
    else if((currCache.hierarchy > opt->spatialIBC->hierarchy) || (stencil->dim == 2))
    {
        double outer_fold=1;

        if(stencil->dim==3)
        {
            outer_fold=stencil->fold_x;
        }
        else
        {
            outer_fold=stencil->fold_y;
        }

        int stencil_contrib_outer = 2*( static_cast<int>((stencil->radius-1)/(outer_fold)) + 1 );
        int stencil_centre = 1;

        double prefetch_oh = (numStencils*(stencil_contrib_outer + stencil_centre) + totalGrids - numStencils)*prefetch_oh_per_LUP;
        double boundary_oh = spatialOh[2]*numStencils;
        double extra_data = boundary_oh+prefetch_oh;
        double actual_data = (numStencils*(stencil_contrib_outer + stencil_centre) + rest_contrib);
        printf("Actual data ILC .....= %f\n", actual_data);
        printf("Rest ILC .......= %f\n", rest_contrib);
        double shrink_space = 1.0 - (extra_data)/(extra_data+actual_data);
        bool ilc_jump = false, olc_jump = false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);
        //double latency_oh = (numStencils*(stencil_contrib_outer + stencil_centre) + totalGrids - numStencils)*latency_oh_per_LUP;
        double streams = numStencils*(stencil_contrib_outer + stencil_centre) + totalGrids - numStencils;
        int rwRatio = static_cast<int>((streams - numWriteGrids)/(double)numWriteGrids);

        int dim =stencil->dim;
        bool jump = (dim>2)?ilc_jump:false;

        if(jump)
        {
            printf("ILC jumped .....\n");
            int stencil_contrib_inner = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_y)) +1 );
            streams = numStencils*(stencil_contrib_outer + stencil_contrib_inner + stencil_centre) + totalGrids - numStencils;
            prefetch_oh = streams*prefetch_oh_per_LUP;
            boundary_oh = spatialOh[3];
            extra_data = prefetch_oh + boundary_oh;
            //    latency_oh = streams*latency_oh_per_LUP;
            rwRatio = static_cast<int>((streams - numWriteGrids)/(double)numWriteGrids);
        }

        double assoc_oh = (dim>2)?assocOh[1]:assocOh[2];

        double totalData = (actual_data + extra_data + assoc_oh);
        ECM_prefetch[currCache.hierarchy] = victim_scale*prefetch_oh*currCache.bytePerWord;
        ECM_boundary[currCache.hierarchy] = victim_scale*boundary_oh*currCache.bytePerWord;
        ECM_assoc[currCache.hierarchy] = assoc_oh*currCache.bytePerWord;

        double latency_oh = getLatencyEffects(currCache, rwRatio);
        ECM_latency[currCache.hierarchy] = latency_oh;
        return {victim_scale*totalData, ((totalData-numWriteGrids)/(double)numWriteGrids)};
    }
    //no blocking condition
    else
    {
        int stencil_contrib_outer = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_x)) +1 );
        int stencil_contrib_inner = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_y)) +1 );
        int stencil_centre = 1;

        double streams = numStencils*(stencil_contrib_outer + stencil_contrib_inner + stencil_centre) + totalGrids - numStencils;
        //ECM_latency[currCache.hierarchy] = (numStencils*(stencil_contrib_outer + stencil_contrib_inner + stencil_centre) + totalGrids - numStencils)*latency_oh_per_LUP;
        double prefetch_oh = streams*prefetch_oh_per_LUP;
        double extra_data = spatialOh[3]+prefetch_oh;
        double actual_data = numStencils*(stencil_contrib_outer + stencil_contrib_inner + stencil_centre) + rest_contrib;
        double shrink_space = 1.0 - (extra_data)/(extra_data+actual_data);
        bool ilc_jump=false, olc_jump=false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);
        double totalData = actual_data + extra_data + assocOh[2];

        ECM_prefetch[currCache.hierarchy] = victim_scale*prefetch_oh*currCache.bytePerWord;
        ECM_boundary[currCache.hierarchy] = victim_scale*spatialOh[3]*currCache.bytePerWord;
        ECM_assoc[currCache.hierarchy] = assocOh[2]*currCache.bytePerWord;

        int rwRatio = static_cast<int>((streams - numWriteGrids)/(double)numWriteGrids);
        double latency_oh = getLatencyEffects(currCache, rwRatio);
        ECM_latency[currCache.hierarchy] = latency_oh;

        return {victim_scale*totalData, ((totalData-numWriteGrids)/(double)numWriteGrids)};
    }
}

//set argument to true if you want to calculate o/h due to temporal blocking
//else false (default) for spatial blocking
std::vector<double> perfModel::addBlockBoundaryEffects(cache_info currCache, bool temporal)
{
    std::vector<double> wordPerLUP;

#ifdef MODEL_BOUNDARY_EFFECT
    int prevHierarchy;

    if((currCache.hierarchy-1) >= 0)
    {
        prevHierarchy = currCache.hierarchy-1;
    }
    else
    {
        prevHierarchy = currCache.hierarchy;
    }


    cache_info cache = CACHES[prevHierarchy];

    //If private cache it will incurr traffic for each block, since there is no
    //sharing
    double bx = (temporal&&cache.shared)?stencil->data->rx:stencil->data->bx;
    double by = (temporal&&cache.shared)?stencil->data->ry:stencil->data->by;
    double bz = (temporal&&cache.shared)?stencil->data->rz:stencil->data->bz;

    double rx = (temporal&&cache.shared)?stencil->data->rx:stencil->data->rx;
    double ry = (temporal&&cache.shared)?stencil->data->ry:stencil->data->ry;
    double rz = (temporal&&cache.shared)?stencil->data->rz:stencil->data->rz;


    double fold_x = stencil->data->fold_x;
    double fold_y = stencil->data->fold_y;
    double fold_z = stencil->data->fold_z;

    bz = std::max(1.0,bz*fold_x*fold_y);
    by = std::max(1.0,ceil(by/fold_y));
    bx = std::max(1.0,ceil(bx/fold_x));

    rz = std::max(1.0,rz*fold_x*fold_y);
    ry = std::max(1.0,ceil(ry/fold_y));
    rx = std::max(1.0,ceil(rx/fold_x));

    int num_block_z = (int)ceil(stencil->data->dz/bz);

    int dim = stencil->dim;

    if(dim!=3)
    {
        bx =1;
    }

    int CL_SIZE = static_cast<int>(cache.cl_size/(double)cache.bytePerWord);
    printf("CL_SIZE = %d\n", CL_SIZE);

    //get the volume of region
    double vol = bx*by*bz;

    int z_scan = std::min(num_block_z, CL_SIZE);

    double cl_avg_z = 0;

    //check for cache line effects in xy face
    for(int i=0; i<z_scan; ++i)
    {
        cl_avg_z += (static_cast<int>((i+1)*bz)%CL_SIZE);
    }
    cl_avg_z=cl_avg_z/z_scan;

    //in temporal blocking we have only 1 face since other comes from cache
    int face = temporal?1:2;

    int thread_x = 1;
    int thread_y = 1;
    int thread_z = 1;

    int totThreads = 1;

    int actualThreads = stencil->data->nthreads;
    if((!temporal && cache.shared) && (totThreads < actualThreads))
    {
        thread_z = std::min(actualThreads, static_cast<int> (rz/bz));
        totThreads = thread_z;
    }
    if((!temporal && cache.shared) && (totThreads < actualThreads))
    {
        thread_y = std::min(actualThreads/thread_z, static_cast<int> (ry/by));
        totThreads += thread_y;
    }
    if((!temporal && cache.shared) && (totThreads < actualThreads))
    {
        thread_x = std::min(actualThreads/(thread_z*thread_y), static_cast<int> (rx/bx));
        totThreads += thread_x;
    }

    double rest_contrib = (numReadGrids-1.0)+numWriteGrids;
    double radius_x, radius_y, radius_z;
    getRadius(radius_x, stencil, x);
    getRadius(radius_y, stencil, y);
    getRadius(radius_z, stencil, z);

    //since inner dimension will be padded
    radius_z *= fold_z;
    radius_x *= fold_x;
    radius_y *= fold_y;

    //temporal blocking has radius 1
    /*if(temporal)
      {
      radius_x = 1;
      radius_y = 1;
      radius_z = 1;
      }*/

    if(dim==3)
    {
        //area of block, with its avg. data contributions
        double data_zy = face*radius_x*(bz)*(by);
        double data_zx = face*radius_y*(bz)*(bx);
        double data_xy = face*radius_z*(bx)*(by);
        //CL effect
        data_xy  += face*cl_avg_z*(bx)*(by);
        printf("cl_avg_z = %f\n", cl_avg_z);
        //Should be if ILC satisfied
        /*if(cache.shared && (radius_y+rest_contrib)*bz*totThreads < cache.getWords()) //shouldn't it be (radius_y+rest_contrib)*bz*thread_z
          {
          data_xy = data_xy/thread_z;
          }
        //Should be if OLC satisfied
        if(cache.shared && (radius_x+rest_contrib)*bz*by*totThreads < cache.getWords())
        {
        data_zx = data_zx/thread_y;
        }
        //Should be if temporal satisfied
        if(cache.shared && (rest_contrib+1)*bz*by*bx*totThreads < cache.getWords())
        {
        data_zy = data_zy/thread_x;
        }*/

        //TODO: this works if and only if spatial blocking and temporal blocking are
        //near to cache size limit; if block tile is too small it might wrap
        //around reducing this effect
        //For caches greater than OLC
        if(cache.shared)
        {
            wordPerLUP.push_back((data_zy/thread_x+data_zx/thread_y+data_xy/thread_z)/vol);
            wordPerLUP.push_back((data_zy+data_zx/thread_y+data_xy/thread_z)/vol);
            //For caches btw OLC and ILC
            wordPerLUP.push_back((data_zx+(2*radius_x+1)*data_xy/thread_z)/vol);
            //For caches less than ILC
            wordPerLUP.push_back(((2*radius_x)+(2*radius_y)+1)*(data_xy)/vol);
        }
        else
        {
            wordPerLUP.push_back((data_zy+data_zx+data_xy)/vol);
            wordPerLUP.push_back((data_zy+data_zx+data_xy)/vol);
            //For caches btw OLC and ILC
            wordPerLUP.push_back((data_zx+(2*radius_x+1)*data_xy)/vol);
            //For caches less than ILC
            wordPerLUP.push_back(((2*radius_x)+(2*radius_y)+1)*(data_xy)/vol);
        }
        printf("data_xy = %f, data_zx = %f, data_zy = %f\n", data_xy, data_zx, data_zy);
    }
    else
    {
        double data_z = face*radius_y*(bz);
        double data_y = face*radius_z*(by);

        data_y += face*cl_avg_z*(by);
        if(cache.shared && (radius_y+rest_contrib)*bz*totThreads < cache.getWords())
        {
            data_y = data_y/thread_z;
        }
        if(cache.shared && (1+rest_contrib)*bz*by*totThreads < cache.getWords())
        {
            data_z = data_z/thread_y;
        }

        wordPerLUP.push_back((data_z+data_y)/vol);
        wordPerLUP.push_back((data_z+data_y)/vol);
        wordPerLUP.push_back((data_z+data_y)/vol);
        wordPerLUP.push_back(((2*radius_y+1)*data_y)/vol);
    }

#else
    UNUSED(currCache);
    UNUSED(temporal);

    wordPerLUP.push_back(0);
    wordPerLUP.push_back(0);
    wordPerLUP.push_back(0);
    wordPerLUP.push_back(0);
#endif
    return wordPerLUP;

}

//words per LUP
double perfModel::getPrefetchEffects(cache_info currCache, blockDetails* opt)
{
#ifdef MODEL_PREFETCH_EFFECT
    bool temporal = false; //there wouldn't be any extra prefetching due to temporal

    int prevHierarchy;

    if((currCache.hierarchy-1) >= 0)
    {
        prevHierarchy = currCache.hierarchy-1;
    }
    else
    {
        prevHierarchy = currCache.hierarchy;
    }
    cache_info prevCache = CACHES[prevHierarchy];

    double bx = stencil->data->bx;
    double by = stencil->data->by;
    double bz = stencil->data->bz;

    double rx = stencil->data->rx;
    double ry = stencil->data->ry;
    double rz = stencil->data->rz;

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

    if(temporal)
    {
        bx = rx;
        by = ry;
        bz = rz;
    }

    int totalGrids = numReadGrids + numWriteGrids;

    int num_block_z = (int)ceil(dz/(((dz-bz)<stencil->data->fold_z)?dz:bz));

    double vol = dx*dy*dz;

    //for entire block overhead
    double prefetch_oh = 0;
    double fold_factor = stencil->data->fold_x*stencil->data->fold_y;

    //    double lhs_boundary_prefetched = std::min(2*stencil->data->radius, currCache.prefetch_cl*8)/dz; //due to lhs
    double reuseFactor = 0;

    int thread_x = 1;
    int thread_y = 1;
    int thread_z = 1;

    int totThreads = 1;

    int actualThreads = stencil->data->nthreads;
    if((totThreads < actualThreads))
    {
        thread_z = std::min(actualThreads, static_cast<int> (rz/bz));
        totThreads = thread_z;
    }
    if((totThreads < actualThreads))
    {
        thread_y = std::min(actualThreads/thread_z, static_cast<int> (ry/by));
        totThreads += thread_y;
    }
    if((totThreads < actualThreads))
    {
        thread_x = std::min(actualThreads/(thread_z*thread_y), static_cast<int> (rx/bx));
        totThreads += thread_x;
    }

    //If only one block in z direction prefetch is reused, only extra prefetching is
    //at boundary
    if( num_block_z == 1 )
    {
        reuseFactor = 1;
    }
    else if(currCache.hierarchy != 0)
    {
        //if one thread and whole block +  it's prefetch(currently not) + single dz fits in upper cache
        //prefetched data is reused
        if(prevCache.shared == false)
        {
            //if more than 1 thread involved each prefetch will contribute as load
            //goes into private cache, and the very next block is assigned to next thread
            if(stencil->data->nthreads == 1)
            {
                if( ((((bx*by+1)*(bz)))*totalGrids) < (prevCache.getWords()*prevCache.sf) )
                {
                    //no prefetch oh
                    reuseFactor = 1;
                }
            }
        }
        else
        {
            //if more than one thread on z dim && upper cache is shared => no prefetching; since data will
            //be reused by neighbouring thread for num_block_z/nthreads blocks
            //No prefetching; because of wrap around
            if( num_block_z == thread_z ) //actualThreads )
            {
                reuseFactor = 1;
            }
            else if( ((((bx*by+1)*(bz)))*totalGrids*actualThreads) < (prevCache.getWords()*prevCache.sf) )
            {
                reuseFactor = 1;
            }
            //if ILC satisfied
            //else if(totalGrids*bz*actualThreads < prevCache.getWords())
            else if(currCache.hierarchy > opt->spatialIBC->hierarchy)
            {
                //for dynamic scheduling on region loop
                reuseFactor = (1- (1/(double)thread_z));
            }
        }
    }

    //TODO:Assuming 8 doubles per CL
    if(stencil->dim==3)
    {
        prefetch_oh= (1-reuseFactor)*(prevCache.prefetch_cl*prevCache.cl_size/(double)prevCache.bytePerWord)*dx*dy*(num_block_z)/fold_factor;
    }
    else
    {
        prefetch_oh= (1-reuseFactor)*(prevCache.prefetch_cl*prevCache.cl_size/(double)prevCache.bytePerWord)*dy*(num_block_z)/fold_factor;
    }


    //return overhead for each LUP
    return (prefetch_oh/vol);
#else
    UNUSED(currCache);
    return 0;
#endif
}

//latency oh in cycles
//latency can happen from spatial blocking in innerDim
//and temporal blocking because both has strided access;
//to get temporal effect set the second argument to true
double perfModel::getLatencyEffects(cache_info currCache, int rwRatio, bool temporal, int temporalNthreads)
{
#ifdef MODEL_LATENCY_EFFECT
    //for entire block overhead
    double latency_oh = 0;

    double bz = temporal ? stencil->data->rz:stencil->data->bz;

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

    int num_block_z = (int)ceil(dz/(((dz-bz)<stencil->data->fold_z)?dz:bz));
    int nthreads = temporal ? temporalNthreads:stencil->data->nthreads;

    double vol = dx*dy*dz;

    double fold_factor = stencil->data->fold_x*stencil->data->fold_y;

    /* bool temporalPass = false;
    //Since with temporal blocking dimension reduces for each time step
    if( ((stencil->data->rx != stencil->data->dx) || (stencil->data->ry != stencil->data->dy) ) || (stencil->data->rz != stencil->data->dz) )
    {
    temporalPass = true;
    }*/
    //if num_block_z == 1 it implies no latency; since it  wraps around
    //correctly
    //temporary will have compulsary latency since the next timestep with next
    //array (alternate between 2 arrays)
    // if((num_block_z>1 || temporal) || temporalPass)
    // if(num_block_z>1)
    //  if( (num_block_z>1) || temporalPass)
    // if((num_block_z>1) || (temporalPass && !temporal))
    if(num_block_z>1)
    {
        if(stencil->dim==3)
        {
            latency_oh=( currCache.getLatency(rwRatio, nthreads)*cpu_freq )*dx*dy*(num_block_z)/fold_factor;
        }
        else
        {
            latency_oh=( currCache.getLatency(rwRatio, nthreads)*cpu_freq )*dy*(num_block_z)/fold_factor;
        }
    }

    latency_oh = latency_oh/vol;
    printf("num_block_z = %d, fold_factor = %f, latency = %f, latency_oh = %f\n", num_block_z, fold_factor, currCache.getLatency(rwRatio, nthreads), latency_oh);
    //return overhead for each LUP
    return (latency_oh);
#else
    UNUSED(currCache);
    UNUSED(temporal);
    return 0;
#endif
}



bool perfModel::LC_violated(cache_info currCache, LC  type)
{
    double blockSize = 0;

    double by_ = stencil->data->by;
    //TODO: Assumed 8 doubles per CL; This is not sufficient more data is
    //prefetched, since every leg should be considered.
    double bz_ = stencil->data->bz;

    if((type==OUTER) && (stencil->dim==3))
    {
        blockSize = by_*bz_;
    }
    else
    {
        blockSize = bz_;
    }

    int nThreads = stencil->data->nthreads;

    double curr_cache_size = currCache.getWords()*currCache.sf;
    if(currCache.shared)
    {
        curr_cache_size=curr_cache_size/((double)nThreads);
    }

    //LC (2*fold*(int((radius-1)/fold) + 1) +1)*size < cache_words
    double layers;
    getLayers(layers,stencil,type);

    if(type == OUTER)
    {
        printf("layers = %f\n", layers);
    }

    //if((layers*blockSize+currCahce.prefetch_cl*8*layers) <= curr_cache_size)
    if(type == INNER)
    {
        printf("layers = %f\n", layers);
        printf("cache size = %f\n", curr_cache_size);
    }
    if((layers*blockSize) <= curr_cache_size)
    {
        return false;
    }
    else
    {
        return true;
    }
}


blockDetails perfModel::determineBlockDetails()
{
    blockDetails opt;

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

    int totalGrids = numReadGrids+numWriteGrids;


    //scan through caches to find the smallest cache that fits entire domain
    for(int i=0; i<((int)CACHES.size()); ++i)
    {
        double curr_cache_size = CACHES[i].getWords()*CACHES[i].sf;

        if( totalGrids*(dx*dy*dz) <= curr_cache_size )
        {
            opt.fit_domain = &CACHES[i];
            break;
        }
    }

    double rx = stencil->data->rx;
    double ry = stencil->data->ry;
    double rz = stencil->data->rz;

    //scan through caches to find the smallest cache that fits regions
    for(int i=0; i<((int)CACHES.size()); ++i)
    {
        double curr_cache_size = CACHES[i].getWords()*CACHES[i].sf;
        //or if private the entire cache size has to fit
        if(!CACHES[i].shared)
        {
            curr_cache_size *= stencil->data->nthreads;
        }
        if( totalGrids*(rx*ry*rz) <= curr_cache_size )
        {
            opt.temporal = &CACHES[i];
            printf("temporal = %s\n",CACHES[i].name.c_str());
            break;
        }
    }

    //find outer LC
    for(int i=0; i<((int)CACHES.size()); ++i)
    {
        if(!LC_violated(CACHES[i], OUTER))
        {
            opt.spatialOBC = &CACHES[i];
            printf("OBC = %s\n",CACHES[i].name.c_str());
            break;
        }
    }

    //find inner LC
    for(int i=0; i<((int)CACHES.size()); ++i)
    {
        if(!LC_violated(CACHES[i], INNER))
        {
            opt.spatialIBC = &CACHES[i];
            printf("IBC = %s\n",CACHES[i].name.c_str());
            break;
        }
    }

    return opt;
}

#if 0
std::vector<double> perfModel::simulateCache_assoc(bool &ilc_jump, bool &olc_jump, cache_info currCache, double shrink)
{
#ifdef MODEL_ASSOCIATIVITY_EFFECT
    ilc_jump = false;
    olc_jump = false;

    int prevHierarchy;

    if((currCache.hierarchy-1) < 0)
    {
        return {0,0,0};
    }
    prevHierarchy = currCache.hierarchy-1;
    cache_info cache = CACHES[prevHierarchy];

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

    double by = stencil->data->by;
    double bz = stencil->data->bz;

    double fold_y = stencil->data->fold_y;
    double fold_x = stencil->data->fold_x;

    int set_size = cache.set_size;

    //decrese set size to contain it's prev cache elements; because they  will
    //be  already cached
    int prevPrevHierarchy;

    if((cache.hierarchy-1) >= 0)
    {
        prevPrevHierarchy = cache.hierarchy-1;
        double ratio = 1-CACHES[prevPrevHierarchy].words/(double)cache.words;
        set_size=static_cast<int>(set_size*ratio);
    }
    if(cache.shared)
    {
        set_size = static_cast<int>(set_size/(double)stencil->data->nthreads);
    }
    set_size = static_cast<int>(shrink*set_size);

    int assoc = cache.ways;

    double centre = dx*dy*dz;
    int r = stencil->radius;
    double centre_rhs = (dx+2*r)*(dy+2*r)*(dz+2*r)+dx*dy*dz;

    //separate different words, in order to avoid counting cache hits as misses
    std::vector<double> words_nlc, words_ilc, words_olc;

    words_nlc.push_back(centre_rhs);
    words_ilc.push_back(centre_rhs);
    words_olc.push_back(centre_rhs);

    double radius_x = ( static_cast<int>((stencil->radius-1)/(fold_x)) + 1 );
    for(int i=(int)(-radius_x); i<=(int)radius_x; ++i)
    {
        if(i!=0)
        {
            double curr_word = dz*dy*i*static_cast<int>(fold_x) + centre;
            words_nlc.push_back(curr_word);
            words_ilc.push_back(curr_word);
            words_olc.push_back(curr_word);
        }
    }

    double radius_y = ( static_cast<int>((stencil->radius-1)/(fold_y)) + 1 );
    double extreme_y = centre;
    for(int i=(int)(-radius_y); i<=(int)radius_y; ++i)
    {
        if(i!=0)
        {
            double curr_word = dz*i*static_cast<int>(fold_y*fold_x) + centre;
            extreme_y = std::max(extreme_y, curr_word);
            words_nlc.push_back(curr_word);
            words_ilc.push_back(curr_word);
            //words_olc.push_back(curr_word); //really not needed but it seems intel repeats event hitted cl
        }
    }
    words_olc.push_back(extreme_y);

    //due to x leg you would load one more cache line till radius 7; after that
    //one more cache line
    int ctr = 1;
    int radius_z = stencil->radius*static_cast<int>(fold_y*fold_x);
    double extreme_z = centre;
    for(int i=0; i<=radius_z; i+=8)
    {
        words_nlc.push_back(centre+ctr);
        //words_ilc.push_back(centre+ctr); //really not needed but it seems intel repeats event hitted cl
        //words_olc.push_back(centre+ctr); //really not needed but it seems intel repeats event hitted cl
        extreme_z = std::max(extreme_z, centre+ctr);
        ++ctr;
    }
    words_ilc.push_back(extreme_z);

    double word_per_cl = 8;
    std::vector<int> cl_ilc;
    std::vector<int> cl_olc;
    std::vector<int> hit_ctr_nlc(set_size,1);
    std::vector<int> hit_ctr_olc(set_size,1);
    std::vector<int> hit_ctr_ilc(set_size,1);

    //Setup initial cache line status for nlc
    for(int i=0; i<(int)words_nlc.size(); ++i)
    {
        int curr_cl = static_cast<int>(words_nlc[i]/word_per_cl)%set_size;
        hit_ctr_nlc[curr_cl] += 1;
    }
    //Setup initial cache line status for ilc
    for(int i=0; i<(int)words_ilc.size(); ++i)
    {
        int curr_cl = static_cast<int>(words_ilc[i]/word_per_cl)%set_size;
        cl_ilc.push_back(curr_cl);
        hit_ctr_ilc[curr_cl] += 1;
    }
    //Setup initial cache line status for olc
    for(int i=0; i<(int)words_olc.size(); ++i)
    {
        int curr_cl = static_cast<int>(words_olc[i]/word_per_cl)%set_size;
        cl_olc.push_back(curr_cl);
        hit_ctr_olc[curr_cl] += 1;
    }

    printf("%s OLC = \n", cache.name.c_str());
    for(int i=0; i<(int)hit_ctr_olc.size(); ++i)
    {
        printf("%d ", hit_ctr_olc[i]);
    }
    printf("\n");

    printf("%s ILC = \n", cache.name.c_str());
    for(int i=0; i<(int)hit_ctr_ilc.size(); ++i)
    {
        printf("%d ", hit_ctr_ilc[i]);
    }
    printf("\n");



    //count extra if no layer condition is satisfied
    double extra_cl_nlc = 0;
    int ctr_nlc = 0;
    for(int i=0; i<(int)hit_ctr_nlc.size(); ++i)
    {
        if(hit_ctr_nlc[i] > assoc)
        {
            extra_cl_nlc += (hit_ctr_nlc[i]-assoc);
            ctr_nlc++;
        }
    }

    if(ctr_nlc > 0)
    {
        PERFWARNING_PRINT("The given dimension might have thrashing effects, please consider to pad them");
    }


    int ilc_layer_cl = static_cast<int>(fold_y*fold_x*(bz/(8.0))+1);
    int olc_layer_cl = static_cast<int>(fold_x*((bz*by)/8.0)+1);

    for(int i=1; i<=ilc_layer_cl; ++i)
    {
        for(int j=0; j<(int)cl_ilc.size(); ++j)
        {
            cl_ilc[j]=(cl_ilc[j]+1)%set_size;
            hit_ctr_ilc[cl_ilc[j]] += 1;
        }
    }

    for(int i=1; i<=olc_layer_cl; ++i)
    {
        for(int j=0; j<(int)cl_olc.size(); ++j)
        {
            cl_olc[j]=(cl_olc[j]+1)%set_size;
            hit_ctr_olc[cl_olc[j]] += 1;
        }
    }

    printf("Cache = %s, ways = %d, sets = %d\n", cache.name.c_str(), cache.ways, (int)hit_ctr_olc.size());
    //count extra cl for olc
    double extra_cl_olc =  0;
    int ctr_olc = 0;
    for(int i=0; i<(int)hit_ctr_olc.size(); ++i)
    {
        printf("%d ", hit_ctr_olc[i]);
        if(hit_ctr_olc[i] > assoc)
        {
            extra_cl_olc += (hit_ctr_olc[i]-assoc);
            ctr_olc++;
        }
    }

    printf("\n");


    /*    for(int i=1; i<=olc_layer_cl; ++i)
          {
          for(int j=0; j<(int)cl_olc.size(); ++j)
          {
          cl_olc[j]=(cl_olc[j]+1)%set_size;
          if(hit_ctr_olc[cl_olc[j]]>assoc)
          {
          extra_cl_olc++;
          ctr_olc++;
          }
          hit_ctr_olc[cl_olc[j]] += 1;
          }
          }
          */

    printf("extra cl olc = %f\n", extra_cl_olc);
    double outer_fold = (stencil->dim==3)?stencil->fold_x:stencil->fold_y;
    int stencil_contrib_outer = 2*( static_cast<int>((stencil->radius-1)/(outer_fold)) + 1 );
    //if more than LC break; record as LC break
    if(extra_cl_olc/static_cast<double>(olc_layer_cl) > stencil_contrib_outer)
    {
        olc_jump = true;
        extra_cl_olc = (stencil_contrib_outer*olc_layer_cl);
    }
    printf("extra cl olc = %f\n", extra_cl_olc);

    //LC broken: naive analytical should have switched at this point
    /* if(ctr_olc == set_size)
       {
       extra_cl_olc = 0;
       }*/

    //printf("Cache = %s, ways = %d, sets = %d\n", cache.name.c_str(), cache.ways, (int)hit_ctr_olc.size());
    //count extra cl for olc
    double extra_cl_ilc =  0;
    int ctr_ilc = 0;
    for(int i=0; i<(int)hit_ctr_ilc.size(); ++i)
    {
        //printf("%d ", hit_ctr_ilc[i]);
        if(hit_ctr_ilc[i] > assoc)
        {
            extra_cl_ilc += (hit_ctr_ilc[i]-assoc);
            ctr_ilc++;
        }
    }

    //printf("\n");

    int stencil_contrib_inner = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_y)) +1 );
    //if more than LC break; record as LC break
    if(extra_cl_ilc/static_cast<double>(ilc_layer_cl) > stencil_contrib_inner)
    {
        ilc_jump = true;
        extra_cl_ilc = (stencil_contrib_inner*ilc_layer_cl);
    }


    /*if(ctr_ilc == set_size)
      {
      extra_cl_ilc = 0;
      }*/

    printf("%f, %f, %f\n", extra_cl_olc, extra_cl_ilc, extra_cl_nlc);
    //extra words per LUP
    return {extra_cl_olc/static_cast<double>(olc_layer_cl), extra_cl_ilc/static_cast<double>(ilc_layer_cl), extra_cl_nlc/1.0};
#else
    UNUSED(currCache);
    return {0,0,0};
#endif
}

#else

//simulates cache associativity and adds the penalty in terms of extra data
//traffic
std::vector<double> perfModel::simulateCache_assoc(bool &ilc_jump, bool &olc_jump, cache_info currCache, double shrink_size)
{
#ifdef MODEL_ASSOCIATIVITY_EFFECT
    double extra_cl_olc = 0;
    double extra_cl_ilc = 0;
    double extra_cl_nlc = 0;
    int r = stencil->radius;

    if(r != 0) //if not streaming code
    {
        ilc_jump = false;
        olc_jump = false;

        int prevHierarchy;

        if((currCache.hierarchy-1) < 0)
        {
            return {0,0,0};
        }
        prevHierarchy = currCache.hierarchy-1;
        cache_info cache = CACHES[prevHierarchy];

        double dx = stencil->data->dx;
        double dy = stencil->data->dy;
        double dz = stencil->data->dz;

        double by = stencil->data->by;
        double bz = stencil->data->bz;

        double fold_y = stencil->data->fold_y;
        double fold_x = stencil->data->fold_x;

        int set_size = cache.set_size;

        //decrese set size to contain it's prev cache elements; because they  will
        //be  already cached
        int prevPrevHierarchy;

        if((cache.hierarchy-1) >= 0)
        {
            prevPrevHierarchy = cache.hierarchy-1;
            double ratio = 1-CACHES[prevPrevHierarchy].getWords()/(double)cache.getWords();
            set_size=static_cast<int>(set_size*ratio);
        }
        if(cache.shared)
        {
            set_size = static_cast<int>(set_size/(double)stencil->data->nthreads);
        }

        set_size = static_cast<int>(set_size*shrink_size);

        int assoc = cache.ways;

        double centre = dx*dy*dz;
        //int r = stencil->radius;
        double centre_rhs = (dx+2*r)*(dy+2*r)*(dz+2*r)+dx*dy*dz;

        //separate different words, in order to avoid counting cache hits as misses
        std::vector<double> words_nlc, words_ilc, words_olc;
        std::vector<double> words_miss_nlc, words_miss_ilc, words_miss_olc;
        std::vector<double> words_hit_nlc, words_hit_ilc, words_hit_olc;
        std::vector<double> words_drop_nlc, words_drop_ilc, words_drop_olc;

        words_miss_nlc.push_back(centre_rhs);
        words_miss_ilc.push_back(centre_rhs);
        words_miss_olc.push_back(centre_rhs);

        int dim = stencil->data->dim;
        double radius_x = (dim > 2)?( static_cast<int>((stencil->radius-1)/(fold_x)) + 1 ):0;
        double centre_next = dx*dy*dz + dy*dz;
        double extreme_x = dz*dy*(radius_x)*static_cast<int>(fold_x) + centre_next;
        double min_x = -1.0*dz*dy*radius_x*static_cast<int>(fold_x) + centre;

        printf("radius_x = %f\n", radius_x);

        if(dim >2)
        {
            for(int i=(int)(-radius_x); i<=(int)radius_x; ++i)
            {
                if(i!=0)
                {
                    double curr_word = dz*dy*i*static_cast<int>(fold_x) + centre;
                    //            words_nlc.push_back(curr_word);
                    //            words_ilc.push_back(curr_word);
                    //           words_drop_nlc.push_back(curr_word);
                    //            words_drop_ilc.push_back(curr_word);
                    words_olc.push_back(curr_word);

                    if(i>(int)(-radius_x))
                    {
                        words_hit_olc.push_back(curr_word);
                    }
                }
            }
            words_drop_olc.push_back(min_x);

            for(int i=(int)(-radius_x); i<=(int)radius_x; ++i)
            {
                if(i!=0)
                {
                    double curr_word = dz*dy*i*static_cast<int>(fold_x) + centre_next;
                    words_miss_nlc.push_back(curr_word);
                    words_miss_ilc.push_back(curr_word);
                }
            }
            words_miss_olc.push_back(extreme_x);
        }

        double radius_y = ( static_cast<int>((stencil->radius-1)/(fold_y)) + 1 );
        centre_next = centre + dz;
        double extreme_y = dz*radius_y*static_cast<int>(fold_y*fold_x) + centre;
        double extreme_y_next = dz*radius_y*static_cast<int>(fold_y*fold_x) + centre_next;
        double min_y = -1.0*dz*radius_y*static_cast<int>(fold_y*fold_x) + centre;
        for(int i=(int)(-radius_y); i<=(int)radius_y; ++i)
        {
            if(i!=0)
            {
                double curr_word = dz*i*static_cast<int>(fold_y*fold_x) + centre;
                //            words_nlc.push_back(curr_word);
                //            words_drop_nlc.push_back(curr_word);
                words_ilc.push_back(curr_word);

                if(i>(int)(-radius_y))
                {
                    words_hit_ilc.push_back(curr_word);
                }
            }
        }
        if(dim > 2)
        {
            words_hit_olc.push_back(extreme_y);
            words_olc.push_back(extreme_y);
        }
        words_drop_ilc.push_back(min_y);

        for(int i=(int)(-radius_y); i<=(int)radius_y; ++i)
        {
            if(i!=0)
            {
                double curr_word = dz*i*static_cast<int>(fold_y*fold_x) + centre_next;
                words_miss_nlc.push_back(curr_word);
            }
        }
        words_miss_ilc.push_back(extreme_y_next);

        //due to x leg you would load one more cache line till radius 7; after that
        //one more cache line
        int radius_z = stencil->radius*static_cast<int>(fold_y*fold_x);
        double extreme_z = centre+radius_z;
        for(int i=0; i<(int)(radius_z/8); i++)
        {
            words_nlc.push_back(centre+i*8);
        }
        words_nlc.push_back(extreme_z);
        words_hit_ilc.push_back(extreme_z);
        words_ilc.push_back(extreme_z);

        double word_per_cl = 8;
        std::vector<int> cl_ilc;
        std::vector<int> cl_olc;
        std::vector<int> cl_miss_ilc;
        std::vector<int> cl_miss_olc;
        std::vector<int> cl_hit_ilc;
        std::vector<int> cl_hit_olc;
        std::vector<int> cl_drop_ilc;
        std::vector<int> cl_drop_olc;

        std::vector<int> way_ctr_nlc(set_size,1);
        std::vector<int> way_ctr_olc(set_size,1);
        std::vector<int> way_ctr_ilc(set_size,1);

        //Setup initial cache line status for nlc
        for(int i=0; i<(int)words_nlc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_nlc[i]/word_per_cl)%set_size;
            way_ctr_nlc[curr_cl] += 1;
        }

        //Setup initial cache line status for ilc
        for(int i=0; i<(int)words_ilc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_ilc[i]/word_per_cl)%set_size;
            cl_ilc.push_back(curr_cl);
            way_ctr_ilc[curr_cl] += 1;
        }
        for(int i=0; i<(int)words_miss_ilc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_miss_ilc[i]/word_per_cl)%set_size;
            cl_miss_ilc.push_back(curr_cl);
        }
        for(int i=0; i<(int)words_hit_ilc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_hit_ilc[i]/word_per_cl)%set_size;
            cl_hit_ilc.push_back(curr_cl);
        }
        for(int i=0; i<(int)words_drop_ilc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_drop_ilc[i]/word_per_cl)%set_size;
            cl_drop_ilc.push_back(curr_cl);
        }

        //Setup initial cache line status for olc
        for(int i=0; i<(int)words_olc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_olc[i]/word_per_cl)%set_size;
            cl_olc.push_back(curr_cl);
            way_ctr_olc[curr_cl] += 1;
        }
        for(int i=0; i<(int)words_miss_olc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_miss_olc[i]/word_per_cl)%set_size;
            cl_miss_olc.push_back(curr_cl);
        }
        for(int i=0; i<(int)words_hit_olc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_hit_olc[i]/word_per_cl)%set_size;
            cl_hit_olc.push_back(curr_cl);
        }
        for(int i=0; i<(int)words_drop_olc.size(); ++i)
        {
            int curr_cl = static_cast<int>(words_drop_olc[i]/word_per_cl)%set_size;
            cl_drop_olc.push_back(curr_cl);
        }

        //count extra if no layer condition is satisfied
        //double extra_cl_nlc = 0;
        int ctr_nlc = 0;
        for(int i=0; i<(int)way_ctr_nlc.size(); ++i)
        {
            if(way_ctr_nlc[i] > assoc)
            {
                extra_cl_nlc += (way_ctr_nlc[i]-assoc);
                ctr_nlc++;
            }
        }

        if(ctr_nlc > 0)
        {
            PERFWARNING_PRINT("The given dimension might have thrashing effects, please consider to pad them");
        }


        int ilc_layer_cl = static_cast<int>(fold_y*fold_x*(bz/(8.0))+1);
        int olc_layer_cl = static_cast<int>(fold_x*((bz*by)/8.0)+1);

        for(int i=1; i<=ilc_layer_cl; ++i)
        {
            for(int j=0; j<(int)cl_ilc.size(); ++j)
            {
                cl_ilc[j]=(cl_ilc[j]+1)%set_size;
                way_ctr_ilc[cl_ilc[j]] += 1;
            }
        }

        //double extra_cl_ilc =  0;
        int ctr_ilc = 0;

        // std::vector<int> cl_dropped_list_ilc;
        for(int i=0; i<ilc_layer_cl; ++i)
        {
            for(int j=0; j<(int)cl_drop_ilc.size(); ++j)
            {
                //cl_dropped_list_ilc.push_back(cl_drop_ilc[j]+i);
                int curr_cl = cl_drop_ilc[j];
                way_ctr_ilc[curr_cl] -= 1;
                cl_drop_ilc[j]=(cl_drop_ilc[j]+1)%set_size;
            }
        }

        //std::sort(cl_dropped_list_ilc.begin(), cl_dropped_list_ilc.end());

        for(int i=1; i<=ilc_layer_cl; ++i)
        {
            for(int j=0; j<(int)cl_miss_ilc.size(); ++j)
            {
                int curr_cl = cl_miss_ilc[j];
                /* if(way_ctr_ilc[curr_cl] == (assoc))
                   {
                   std::vector<int>::iterator pos = std::lower_bound(cl_dropped_list_ilc.begin(), cl_dropped_list_ilc.end(), curr_cl);
                   if(pos != cl_dropped_list_ilc.end())
                   {
                //drop this element
                cl_dropped_list_ilc.erase(pos);
                }
                else
                {
                way_ctr_ilc[curr_cl] += 1;
                ++extra_cl_ilc;
                ++ctr_ilc;
                }
                }*/
                if(way_ctr_ilc[curr_cl] >= (assoc))
                {
                    /*
                       std::vector<int>::iterator pos = std::lower_bound(cl_dropped_list_ilc.begin(), cl_dropped_list_ilc.end(), curr_cl);
                       if(pos != cl_dropped_list_ilc.end())
                       {
                    //drop this element
                    cl_dropped_list_ilc.erase(pos);
                    }
                    else*/
                    {
                        way_ctr_ilc[curr_cl] += 1;
                    }
                    ++extra_cl_ilc;
                    ++ctr_ilc;
                }
                else
                {
                    way_ctr_ilc[curr_cl] += 1;
                }

                cl_miss_ilc[j]=(cl_miss_ilc[j]+1)%set_size;
            }

            for(int j=0; j<(int)cl_hit_ilc.size(); ++j)
            {
                int curr_cl = cl_hit_ilc[j];
                if(way_ctr_ilc[curr_cl] > assoc)
                {
                    /*      std::vector<int>::iterator pos = std::lower_bound(cl_dropped_list_ilc.begin(), cl_dropped_list_ilc.end(), curr_cl);
                            if(pos != cl_dropped_list_ilc.end())
                            {
                    //drop this element
                    cl_dropped_list_ilc.erase(pos);
                    }
                    else*/
                    {
                        way_ctr_ilc[curr_cl] += 1;
                    }
                    ++extra_cl_ilc;
                    ++ctr_ilc;
                }
                cl_hit_ilc[j]=(cl_hit_ilc[j]+1)%set_size;
            }
            int stencil_contrib_inner = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_y)) +1 );
            //if more than LC break; record as LC break
            if(extra_cl_ilc/static_cast<double>(ilc_layer_cl) > stencil_contrib_inner)
            {
                extra_cl_ilc = (stencil_contrib_inner*ilc_layer_cl);
                ilc_jump = true;
            }
        }
        printf("extra cl ilc = %f\n", extra_cl_ilc);


        for(int i=1; i<=olc_layer_cl; ++i)
        {
            for(int j=0; j<(int)cl_olc.size(); ++j)
            {
                cl_olc[j]=(cl_olc[j]+1)%set_size;
                way_ctr_olc[cl_olc[j]] += 1;
            }
        }

        //double extra_cl_olc =  0;
        int ctr_olc = 0;

        if(dim > 2)
        {
            //std::vector<int> cl_dropped_list_olc;
            for(int i=0; i<olc_layer_cl; ++i)
            {
                for(int j=0; j<(int)cl_drop_olc.size(); ++j)
                {
                    // cl_dropped_list_olc.push_back(cl_drop_olc[j]+i);
                    int curr_cl = cl_drop_olc[j];
                    way_ctr_olc[curr_cl] -= 1;
                    cl_drop_olc[j]=(cl_drop_olc[j]+1)%set_size;
                }
            }


            //std::sort(cl_dropped_list_olc.begin(), cl_dropped_list_olc.end());

            for(int i=1; i<=olc_layer_cl; ++i)
            {
                for(int j=0; j<(int)cl_miss_olc.size(); ++j)
                {
                    int curr_cl = cl_miss_olc[j];
                    /*if(way_ctr_olc[curr_cl] == (assoc))
                      {
                      std::vector<int>::iterator pos = std::lower_bound(cl_dropped_list_olc.begin(), cl_dropped_list_olc.end(), curr_cl);
                      if(pos != cl_dropped_list_olc.end())
                      {
                    //drop this element
                    cl_dropped_list_olc.erase(pos);
                    }
                    else
                    {
                    way_ctr_olc[curr_cl] += 1;
                    ++extra_cl_olc;
                    ++ctr_olc;
                    }
                    }*/
                    if(way_ctr_olc[curr_cl] >= (assoc))
                    {
                        /* std::vector<int>::iterator pos = std::lower_bound(cl_dropped_list_olc.begin(), cl_dropped_list_olc.end(), curr_cl);
                           if(pos != cl_dropped_list_olc.end())
                           {
                        //drop this element
                        cl_dropped_list_olc.erase(pos);
                        }
                        else*/
                        {
                            way_ctr_olc[curr_cl] += 1;
                        }
                        ++extra_cl_olc;
                        ++ctr_olc;
                    }
                    else
                    {
                        way_ctr_olc[curr_cl] += 1;
                    }
                    cl_miss_olc[j]=(cl_miss_olc[j]+1)%set_size;
                }

                for(int j=0; j<(int)cl_hit_olc.size(); ++j)
                {
                    int curr_cl = cl_hit_olc[j];
                    if(way_ctr_olc[curr_cl] > assoc)
                    {
                        /* std::vector<int>::iterator pos = std::lower_bound(cl_dropped_list_olc.begin(), cl_dropped_list_olc.end(), curr_cl);
                           if(pos != cl_dropped_list_olc.end())
                           {
                        //drop this element
                        cl_dropped_list_olc.erase(pos);
                        }
                        else*/
                        {
                            way_ctr_olc[curr_cl] += 1;
                        }
                        ++extra_cl_olc;
                        ++ctr_olc;
                    }
                    cl_hit_olc[j]=(cl_hit_olc[j]+1)%set_size;
                }
                double outer_fold = (stencil->dim==3)?stencil->fold_x:stencil->fold_y;
                int stencil_contrib_outer = 2*( static_cast<int>((stencil->radius-1)/(outer_fold)) + 1 );
                //printf("lhs = %f, rhs = %d\n", extra_cl_olc/static_cast<double>(olc_layer_cl), stencil_contrib_outer);
                //if more than LC break; record as LC break
                if(extra_cl_olc/static_cast<double>(olc_layer_cl) > stencil_contrib_outer)
                {
                    extra_cl_olc = (stencil_contrib_outer*olc_layer_cl);
                    olc_jump = true;
                    break;
                }
            }
        }
        printf("extra cl olc = %f\n", extra_cl_olc);


        printf("Cache = %s, ways = %d, sets = %d\n", cache.name.c_str(), cache.ways, (int)way_ctr_olc.size());
        //count extra cl for olc
        /*    double extra_cl_olc =  0;
              int ctr_olc = 0;
              for(int i=0; i<(int)way_ctr_olc.size(); ++i)
              {
              printf("%d ", way_ctr_olc[i]);
              if(way_ctr_olc[i] > assoc)
              {
              extra_cl_olc += (way_ctr_olc[i]-assoc);
              ctr_olc++;
              }
              }

              printf("\n");
              printf("extra cl olc = %f\n", extra_cl_olc);
              */
        //LC broken: naive analytical should have switched at this point
        /* if(ctr_olc == set_size)
           {
           extra_cl_olc = 0;
           }*/

        //printf("Cache = %s, ways = %d, sets = %d\n", cache.name.c_str(), cache.ways, (int)way_ctr_olc.size());
        //count extra cl for olc
        /*    double extra_cl_ilc =  0;
              int ctr_ilc = 0;
              for(int i=0; i<(int)way_ctr_ilc.size(); ++i)
              {
        //printf("%d ", way_ctr_ilc[i]);
        if(way_ctr_ilc[i] > assoc)
        {
        extra_cl_ilc += (way_ctr_ilc[i]-assoc);
        ctr_ilc++;
        }
        }
        */
        //printf("\n");


        /*if(ctr_ilc == set_size)
          {
          extra_cl_ilc = 0;
          }*/

        printf("%f, %f, %f\n", extra_cl_olc, extra_cl_ilc, extra_cl_nlc);
        //extra words per LUP
        return {extra_cl_olc/static_cast<double>(olc_layer_cl), extra_cl_ilc/static_cast<double>(ilc_layer_cl), extra_cl_nlc/1.0};
    }
    else
    {
        extra_cl_olc = 0;
        extra_cl_ilc = 0;
        extra_cl_nlc = 0;

        return {0,0,0};
    }
#else
    UNUSED(currCache);
    UNUSED(ilc_jump);
    UNUSED(olc_jump);
    UNUSED(shrink_size);
    return {0,0,0};
#endif
}
#endif


//we give it as a ratio of extra work to current work
double perfModel::calcTemporalExtraWork()
{
    double bx = stencil->data->bx;
    double by = stencil->data->by;
    double bz = stencil->data->bz;

    double rx = stencil->data->rx;
    double ry = stencil->data->ry;
    double rz = stencil->data->rz;

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

    int totThreads = 1;
    int thread_x = 1, thread_y=1, thread_z=1;

    int actualThreads = stencil->data->nthreads;
    // int remThreads = actualThreads;

    thread_z =  tolCeil(rz/bz);//std::min(remThreads, tolCeil(rz/bz));
    /*if((remThreads%thread_z)!=0)
      {
      thread_z = 1;
      }*/
    totThreads = thread_z;
    // remThreads = actualThreads/totThreads;

    if((totThreads % actualThreads)!=0)
    {
        thread_y = tolCeil(ry/by);//std::min(remThreads, tolCeil(ry/by));
        /*if((remThreads%thread_y)!=0)
          {
          thread_y = 1;
          }*/
        totThreads *= thread_y;
        //remThreads = actualThreads/totThreads;

        if((totThreads % actualThreads)!=0)
        {
            thread_x = tolCeil(rx/bx);//std::min(remThreads, tolCeil(rx/bx));
            /*if((remThreads%thread_x)!=0)
              {
              thread_x = 1;
              }*/
            totThreads*=thread_x;
        }
    }

    //Only case that is not checked
    if((thread_x*thread_y)%actualThreads == 0)
    {
        thread_z = 1;
    }

    totThreads = thread_x*thread_y*thread_z;

    if( (totThreads%actualThreads) != 0)
    {
        ERROR_PRINT("Partition not ideal for %d threads, we have %d partitions", actualThreads, totThreads);
    }

    double dt = stencil->data->dt;

    double extraWork = 0;

    int angle_x = 0, angle_y = 0, angle_z = 0;

    if((int)dx!=(int)rx)
    {
        getRadiusFolded(angle_x, stencil->data, x);
    }
    if((int)dy!=(int)ry)
    {
        getRadiusFolded(angle_y, stencil->data, y);
    }
    if((int)dz!=(int)rz)
    {
        getRadiusFolded(angle_z, stencil->data, z);
    }

    double extraWork_x = 0, extraWork_y = 0, extraWork_z = 0;

    //x direction
    //first part till min(dt, bx)
    if(angle_x != 0)
    {
        int first_part = std::min((int)(dt-1), (int)(bx/(double)angle_x));
        extraWork_x = angle_x*thread_x*first_part*(first_part+1)/2.0;
        printf("check .... EW %f\n", extraWork_x);
        int second_part = (int)(dt-1) - first_part;
        extraWork_x += (second_part*thread_x*bx);
        printf("check .... EW %f\n", extraWork_x);
    }

    if(angle_y != 0)
    {
        //y direction
        int first_part = std::min((int)(dt-1), (int)(by/(double)angle_y));
        extraWork_y = angle_y*thread_y*first_part*(first_part+1)/2.0;

        int second_part = (int)(dt-1) - first_part;
        extraWork_y += (second_part*thread_y*by);
    }

    if(angle_z != 0)
    {
        //z direction
        int first_part = std::min((int)(dt-1), (int)(bz/(double)angle_z));
        extraWork_z = angle_z*thread_z*first_part*(first_part+1)/2.0;

        int second_part = (int)(dt-1) - first_part;
        extraWork_z += (second_part*thread_z*bz);
    }

    extraWork = extraWork_x/dx + extraWork_y/dy + extraWork_z/dz;

    extraWork = extraWork/(double)dt;

    return extraWork;
}

//Does an ECM modelling for given stencil
//returns the MEM->L3 bw chosen
void perfModel::calc_ECM(int scale, int temporalStoreMode)
{

    int bytePerWord = CACHES[0].bytePerWord;

    blockDetails opt = determineBlockDetails();

    //    spatialOh = addBlockBoundaryEffects();
    //    temporalOh = addBlockBoundaryEffects(true);
    //printf("Spatial Blocking o/h outer = %f inner = %f\n", spatialOh[0], spatialOh[1]);
    //printf("Temporal Blocking o/h outer = %f inner = %f\n", temporalOh[0], temporalOh[1]);

    ECM.clear();
    ECM_data.clear();
    ECM_boundary_cy.clear();
    ECM_prefetch_cy.clear();
    ECM_assoc_cy.clear();

    //block throughput and L1->core is for fold_x*fold_y*fold_z iterations
    int core_factor = stencil->fold_x*stencil->fold_y*stencil->fold_z;
    int data_factor = 1;

    //1st core contribution
    ECM.push_back((blockThroughput/core_factor)*scale);
    //L1->core
    double l1_core=0.0;
    for(int port_idx=0; port_idx<(int)LD_ports.size(); ++port_idx)
    {
        l1_core = std::max(l1_core, portCycle[LD_ports[port_idx]]);
    }
    ECM.push_back((l1_core/core_factor)*scale);

    //calculate latency, data transfer from IACA
    std::vector<double> l1_core_data = getDataContrib("L1",&opt,temporalStoreMode);
    ECM_boundary[0] = 0;
    ECM_prefetch[0] = 0;
    ECM_assoc[0] = 0;
    ECM_prefetch_cy.push_back(0);
    ECM_boundary_cy.push_back(0);
    ECM_assoc_cy.push_back(0);

    int nthreads = stencil->data->nthreads;

    //L2->L1
    std::vector<double> l2_l1_data = getDataContrib("L2",&opt,temporalStoreMode);
    double l2_l1_transfer_rate = CACHE("L2").getBytePerCycle((int)l2_l1_data[1], nthreads); //bytePerCycle["L2"][0];
    bytePerCycle["L2"].push_back(l2_l1_transfer_rate);
    double l2_l1 = (l2_l1_data[0]*bytePerWord)/l2_l1_transfer_rate;
    ECM.push_back((l2_l1/data_factor)*scale);
    ECM_data.push_back(l2_l1_data[0]*bytePerWord);
    ECM_prefetch_cy.push_back(ECM_prefetch[1]*scale/l2_l1_transfer_rate);
    ECM_boundary_cy.push_back(ECM_boundary[1]*scale/l2_l1_transfer_rate);
    ECM_assoc_cy.push_back(ECM_assoc[1]*scale/l2_l1_transfer_rate);
    ECM_latency[1] += (ECM_latency[1]+l2_l1)*CACHE("L2").penalty;

    //L3->L2
    std::vector<double> l3_l2_data = getDataContrib("L3",&opt,temporalStoreMode);
    double l3_l2_transfer_rate = CACHE("L3").getBytePerCycle((int)l3_l2_data[1], nthreads);//bytePerCycle["L3"][0];
    bytePerCycle["L3"].push_back(l3_l2_transfer_rate);
    printf("L3 transfer rate = %f\n", l3_l2_transfer_rate);
    double l3_l2 = (l3_l2_data[0]*bytePerWord)/l3_l2_transfer_rate;
    ECM.push_back((l3_l2/data_factor)*scale);
    ECM_data.push_back(l3_l2_data[0]*bytePerWord);
    ECM_prefetch_cy.push_back(ECM_prefetch[2]*scale/l3_l2_transfer_rate);
    ECM_boundary_cy.push_back(ECM_boundary[2]*scale/l3_l2_transfer_rate);
    ECM_assoc_cy.push_back(ECM_assoc[2]*scale/l3_l2_transfer_rate);
    ECM_latency[2] += (ECM_latency[2]+l3_l2)*CACHE("L3").penalty;

    //MEM->L3
    std::vector<double> mem_l3_data = getDataContrib("MEM",&opt,temporalStoreMode);
    //int bw_idx = (int)(round(mem_l3_data[1]))-1;
    //bw_idx = (bw_idx<0)?0:bw_idx;
    //restrict to ratio 9
    //bw_idx = std::min(9,bw_idx);
    chosen_mem_l3_bw = CACHE("MEM").getBytePerCycle((int)mem_l3_data[1], nthreads);
    double mem_l3_transfer_rate = chosen_mem_l3_bw;//bytePerCycle["MEM"][bw_idx];
    bytePerCycle["MEM"].push_back(mem_l3_transfer_rate);
    double mem_l3 = (mem_l3_data[0]*bytePerWord)/mem_l3_transfer_rate;
    ECM.push_back((mem_l3/data_factor)*scale);
    ECM_data.push_back(mem_l3_data[0]*bytePerWord);
    ECM_prefetch_cy.push_back(ECM_prefetch[3]*scale/mem_l3_transfer_rate);
    ECM_boundary_cy.push_back(ECM_boundary[3]*scale/mem_l3_transfer_rate);
    ECM_assoc_cy.push_back(ECM_assoc[3]*scale/mem_l3_transfer_rate);
    ECM_latency[3] += (ECM_latency[3]+mem_l3)*CACHE("MEM").penalty;

    chosen_mem_l3_bw = chosen_mem_l3_bw*cpu_freq;
}

void perfModel::model(int scale, bool validate_)
{
    //for derived calc is done in + operator
    if(!derived)
    {
        bool temporal = (stencil->data->rt > 1)?true:false;
        if(temporal)
        {
            //if temporal is true we have to model using 3 phases
            //the burst phase, in-cache phase, store phase.
            //This is because saturation behaviour of these phases are different
            int orig_dt = stencil->data->dt;

            //2 phase or 3 phase doesn't really matter as long as appropriate
            //MEM bandwidth is considered
            //model burst phase
            stencil->data->dt = 1;
            calc_ECM(scale);
            double burst_sat = getSaturation()[0];
            double burst_cy = applyNOL();
            printf(".......... burst sat = %f............\n", burst_sat);
            //now model in-cache phase
            stencil->data->dt = 10000000; //a big number so it doesn't have any memory effect
            calc_ECM(scale);
            double inCache_sat = getSaturation()[0];
            double inCache_cy = applyNOL();
            /*
            //now model store phase
            bytePerCycle["MEM"] = 8.43;
            stencil->data->dt = 1;
            calc_ECM(scale,2);
            double store_sat = getSaturation();
            double store_cy = applyNOL();
            bytePerCycle["MEM"] = 19;
            */
            //now combine the results to get a penalty value
            //for 2 phases
            stencil->data->dt = orig_dt;
            calc_ECM(scale);
            double sat = getSaturation()[0];
            double cy  = applyNOL();

            /*printf("burst: cy = %f sat = %f\n", burst_cy, burst_sat);
              printf("inCache: cy = %f sat = %f\n", inCache_cy, inCache_sat);
              printf("even: cy = %f sat = %f\n", cy, sat);*/
            //calculate penalty
            //double actual_cy = sat*(1.0*burst_cy/burst_sat + (orig_dt-2.0)*inCache_cy/inCache_sat + 1.0*store_cy/store_sat)/((double)orig_dt);
            double actual_cy = sat*(1.0*burst_cy/burst_sat + (orig_dt-1.0)*inCache_cy/inCache_sat)/((double)orig_dt);
            ECM_penalty = (actual_cy - cy);

            ECM_extra_work_percent = calcTemporalExtraWork();
        }
        else
        {
            calc_ECM(scale);
        }
    }

    if(validate_)
    {
        validate();
        val = true;
    }
}


void printECM_style(std::vector<double> ECM, std::vector<double> ECM_data, double ECM_penalty, std::vector<double> ECM_latency, std::vector<double> ECM_prefetch, std::vector<double> ECM_boundary, std::vector<double> ECM_assoc, std::vector<double> ECM_prefetch_cy, std::vector<double> ECM_boundary_cy, std::vector<double> ECM_assoc_cy)
{
    printf("\n%12s : [ %5.2f || ", "Throughput", ECM[0]);
    for(int i=1; i<((int)ECM.size()-1); ++i)
    {
        printf("%5.2f | ", ECM[i]);
    }
    printf("%5.2f ] ", ECM[ECM.size()-1]);

    if(ECM_penalty  > 1e-10)
    {
        printf("+ %5.2f cy/LUP\n", ECM_penalty);
    }
    else
    {
        printf("cy/LUP\n");
    }


    if(!ECM_latency.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Latency" ,  "N/A", ECM_latency[0]);
        for(int i=1; i<((int)ECM_latency.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_latency[i]);
        }
        printf("%5.2f ] cy/LUP\n", ECM_latency[ECM_latency.size()-1]);
    }

    if(!ECM_prefetch_cy.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Prefetch" ,  "N/A", ECM_prefetch_cy[0]);
        for(int i=1; i<((int)ECM_prefetch_cy.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_prefetch_cy[i]);
        }
        printf("%5.2f ] cy/LUP\n", ECM_prefetch_cy[ECM_prefetch.size()-1]);
    }

    if(!ECM_boundary_cy.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Boundary" ,  "N/A", ECM_boundary_cy[0]);
        for(int i=1; i<((int)ECM_boundary_cy.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_boundary_cy[i]);
        }
        printf("%5.2f ] cy/LUP\n", ECM_boundary_cy[ECM_boundary.size()-1]);
    }

    if(!ECM_assoc_cy.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Assoc" ,  "N/A", ECM_assoc_cy[0]);
        for(int i=1; i<((int)ECM_assoc_cy.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_assoc_cy[i]);
        }
        printf("%5.2f ] cy/LUP\n", ECM_assoc_cy[ECM_assoc.size()-1]);
    }


    if(!ECM_prefetch.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Prefetch" ,  "N/A", ECM_prefetch[0]);
        for(int i=1; i<((int)ECM_prefetch.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_prefetch[i]);
        }
        printf("%5.2f ] bytes/LUP\n", ECM_prefetch[ECM_prefetch.size()-1]);
    }

    if(!ECM_boundary.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Boundary" ,  "N/A", ECM_boundary[0]);
        for(int i=1; i<((int)ECM_boundary.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_boundary[i]);
        }
        printf("%5.2f ] bytes/LUP\n", ECM_boundary[ECM_boundary.size()-1]);
    }

    if(!ECM_assoc.empty())
    {
        printf("%12s : [ %5s || %5.2f | ", "Assoc" ,  "N/A", ECM_assoc[0]);
        for(int i=1; i<((int)ECM_assoc.size()-1); ++i)
        {
            printf("%5.2f | ", ECM_assoc[i]);
        }
        printf("%5.2f ] bytes/LUP\n", ECM_assoc[ECM_assoc.size()-1]);
    }

    printf("%12s : [ %5s || %5s | ", "Data", "N/A", "N/A");
    for(int i=0; i<(int)(ECM_data.size()-1); ++i)
    {
        printf("%5.2f | ", ECM_data[i]);
    }
    printf("%5.2f ] bytes/LUP\n", ECM_data[ECM_data.size()-1]);
}

void perfModel::printECM(const char* title)
{
    //model(1);

    printf("\nECM Model: %s\n", title);
    printECM_style(ECM, ECM_data, ECM_penalty, ECM_latency, ECM_prefetch, ECM_boundary, ECM_assoc, ECM_prefetch_cy, ECM_boundary_cy, ECM_assoc_cy);
    printf("Chosen MEM->L3 bandwidth = %f\n", chosen_mem_l3_bw);

    //double perf=applyNOL();
    double sat = getSaturation()[0];

    printf("Saturation @ %5.2f\n",sat);
    printf("ECM extra work = %5.2f%\n", ECM_extra_work_percent*100);
    //convert cy/LUP to MLUP/s
    double mlups = getPerf();
    printf("ECM Perf = %f MLUP/s\n\n", mlups);

    if(val)
    {
        printf("Validation\n");
        //no penalty in validation
        printECM_style(ECM_validate, ECM_validate_data, 0);
    }

    printf("\n\n");
}

double utilization(double nthreads, double tcomp, double tdata, double tL3Mem)
{
    return 1; //Else for different machines will have to measure
    double p0 = 0.6*tL3Mem;
    double value = 0;

    if(nthreads==0)
    {
        return 0;
    }
    else
    {
        value = (nthreads*tL3Mem)/ std::max(tcomp, tdata + utilization(nthreads-1, tcomp, tdata, tL3Mem)*(nthreads-1)*p0);
    }

    if(value > 1)
    {
        return 1;
    }
    else
    {
        return value;
    }
}

//To fix by splitting nthreads_x, nthreads_y, nthreads_z
//i.e. reminder in y can be divided by nthreads_z threads
std::vector<double> perfModel::getSaturation()
{
    double totalNOL = applyNOL()-ECM_penalty; //because penalty was calculated with previous saturation
    double sat = 0;
    double mem_l3 = ECM.back();
    double nthreads = stencil->data->nthreads;

    //calculate effective threads
    double nBlocks = ((DOUBLE(stencil->data->rx) *DOUBLE(stencil->data->ry)*DOUBLE(stencil->data->rz)) / (DOUBLE(stencil->data->bx)*DOUBLE(stencil->data->by)*DOUBLE(stencil->data->bz)));
    double effThreads = nthreads;
    printf("nBlocks = %f\n", nBlocks);

#ifdef MODEL_REMAINDER_EFFECT
    if( nthreads > 1)
    {
        if(nBlocks > nthreads)
        {
            double extra_work = fmod(nBlocks,nthreads);
            double extra_work_full_threads = static_cast<int>(extra_work);
            double max_work = 0;
            double min_work = 0;
            if(static_cast<int>(extra_work_full_threads) == (static_cast<int>(nthreads)-1))
            {
                max_work = 1;
                min_work = (extra_work-extra_work_full_threads);
            }
            else if(static_cast<int>(extra_work_full_threads)==0)
            {
                max_work = (extra_work-extra_work_full_threads);
                min_work = 0;
            }
            else
            {
                max_work = 1;
                min_work = 0;
            }

            double thread_multiple = static_cast<int>(nBlocks/nthreads);
            effThreads = nthreads/(1+((max_work-min_work)/thread_multiple));
        }
        else
        {
            effThreads = nBlocks;
        }
    }
#endif
    double tcomp = ECM[0];
    double util = utilization(nthreads, tcomp, totalNOL, mem_l3);
    printf("....util = %f, tnol/mem = %f\n", util, totalNOL/mem_l3);

    printf("effThreads= %f\n", effThreads);
    if(effThreads > nthreads)
    {
        ERROR_PRINT("This shouldn't have happened\n");
    }

    if(mem_l3 < 1e-5)
    {
        sat = nthreads;
    }
    else
    {
        sat = util*(totalNOL/mem_l3);
        sat = std::min(sat,effThreads);
    }

    return {sat, totalNOL/mem_l3, effThreads};
}


//assumption: NOL
double perfModel::applyNOL()
{
    double nOL = 0;
    for(int i=1; i<((int)ECM.size()); ++i)
    {
        nOL+=ECM[i];
    }
    nOL+=ECM_penalty;


    double nOL_latency = 0;
    //add latency contributions
    for(int i=0; i<((int)ECM_latency.size()); ++i)
    {
        nOL_latency+=ECM_latency[i];
    }

    double nOL_prefetch = 0;
    //add prefetch contribution (this is just for getting statistics)
    for(int i=0; i<((int)ECM_prefetch_cy.size()); ++i)
    {
        nOL_prefetch+=ECM_prefetch_cy[i];
    }

    double nOL_boundary = 0;
    //add boundary contribution (this is just for getting statistics)
    for(int i=0; i<((int)ECM_boundary_cy.size()); ++i)
    {
        nOL_boundary+=ECM_boundary_cy[i];
    }

    double nOL_assoc = 0;
    //add boundary contribution (this is just for getting statistics)
    for(int i=0; i<((int)ECM_assoc_cy.size()); ++i)
    {
        nOL_assoc+=ECM_assoc_cy[i];
    }

    double total_nOL = nOL+nOL_latency;
    double perf = 0;
    if(total_nOL < ECM[0])
    {
        ecm_contribution.latency = 0;
        ecm_contribution.boundary = 0;
        ecm_contribution.prefetch = 0;
        ecm_contribution.associativity = 0;
        //TODO check latency contribution in OL part
        perf = ECM[0];
    }
    else
    {
        ecm_contribution.latency = nOL_latency/total_nOL;
        ecm_contribution.boundary = nOL_boundary/total_nOL;
        ecm_contribution.prefetch = nOL_prefetch/total_nOL;
        ecm_contribution.associativity = nOL_assoc/total_nOL;
        perf = nOL+nOL_latency;
    }
    return perf;
}

//in Mlup/s
double perfModel::getPerf()
{
    double perf = applyNOL();
    double sat = getSaturation()[0];
    double mlups = sat*(cpu_freq/perf)*1000.0;
    mlups = mlups/(1+ECM_extra_work_percent);
    return mlups;
}

std::vector<double> perfModel::readFreq(int i)
{
    int nthreads = stencil->data->nthreads;
    double result = 0;
    double avg = 0;
    double max = 0;
    double min = DBL_MAX;

    for(int thread=0; thread<nthreads; ++thread)
    {
        result = perfmon_getLastMetric(gid[i], metric_id[i], cpus[thread]);
        avg+=result;
        max=std::max(max,result);
        min=std::min(min,result);
    }

    avg=avg/nthreads;

    return {min, avg, max};
}

double perfModel::readDataVol(int i)
{
    int nthreads = stencil->data->nthreads;
    double result = 0;

    for(int thread=0; thread<nthreads; ++thread)
    {
        result += perfmon_getLastMetric(gid[i], metric_id[i], cpus[thread]);
    }
    result=result*(1000*1000*1000.0);

    return result;
}

/*double perfModel::readDataVol(int eOffsetId)
  {
  int nthreads = stencil->data->nthreads;
  double result = 0;

  for(int j = eventOffset[eOffsetId]; j < eventOffset[eOffsetId+1];  ++j)
  {
  for (int i = 0;i < nthreads; i++)
  {
  result += perfmon_getLastResult(gid, j, i);
  }
  }
  result*=(64);
  return result;
  }*/

void setCounter(std::string groupName, std::string metricName, int *curr_gid, int *curr_metric_id)
{
    (*curr_gid) = perfmon_addEventSet(groupName.c_str());

    int numMetrics = perfmon_getNumberOfMetrics(*curr_gid);
    std::string toFind(metricName);
    int dataVol_metric_id = numMetrics-1;
    for(int i=0; i<numMetrics; ++i)
    {
        std::string currMetric(perfmon_getMetricName(*curr_gid,i));
        if(currMetric.find(toFind) != std::string::npos)
        {
            printf("Found %s\n", currMetric.c_str());
            dataVol_metric_id = i;
            break;
        }
    }
    (*curr_metric_id) = dataVol_metric_id;
}

void perfModel::validate()
{

    double dt = stencil->data->dt;
    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;
    double LUP = dt*dx*dy*dz;

    int nthreads = stencil->data->nthreads;

    if(!likwidInited)
    {
        /*Likwid initialization*/
        topology_init();

        CpuTopology_t topo = get_cpuTopology();
        affinity_init();

        cpus = (int*)malloc(nthreads * sizeof(int));

        for(int i=0;i<nthreads;i++)
        {
            cpus[i] = topo->threadPool[i].apicId;
        }

        perfmon_init(nthreads, cpus);

        //initialise required counters

        likwidInited = true;

        /*
        //validate using LIKWID
        char* mem_event;
        char* curr_str;
        int num_mbox=8;
        int currOffset = 0;

        eventOffset.push_back(currOffset);

        char* l2_event;
        STRINGIFY(l2_event, "L1D_REPLACEMENT:PMC0,L1D_M_EVICT:PMC1");
        currOffset+=2;
        eventOffset.push_back(currOffset);

        char* l3_event;
        STRINGIFY(l3_event,"L2_LINES_IN_ALL:PMC2,L2_LINES_OUT_DIRTY_ALL:PMC3");
        currOffset+=2;
        eventOffset.push_back(currOffset);

        STRINGIFY(curr_str,"CAS_COUNT_RD:MBOX0C0,CAS_COUNT_WR:MBOX0C1");

        for(int mbox=1; mbox<num_mbox; ++mbox)
        {
        STRINGIFY(mem_event,"%s,CAS_COUNT_RD:MBOX%dC0,CAS_COUNT_WR:MBOX%dC1",curr_str,mbox,mbox);
        char* tmp = curr_str;
        curr_str = mem_event;
        mem_event = tmp;
        }
        free(mem_event);
        mem_event=curr_str;
        currOffset += 2*num_mbox;
        eventOffset.push_back(currOffset);

        char* event_str;
        STRINGIFY(event_str, "%s,%s,%s", l2_event, l3_event, mem_event);

        int curr_gid = perfmon_addEventSet("MEM");
        perfmon_setupCounters(curr_gid);
        */

        int curr_gid = 0;
        int curr_metric_id = 0;
        //gName = std::vector<std::string>({"L2", "L3", "MEM", "CLOCK"});
        gName = std::vector<std::string>({"L2", "L3", "MEM"});
        //metricName = std::vector<std::string>({"L2 data volume", "L3 data volume", "Memory data volume", "Clock"});
        metricName = std::vector<std::string>({"L2 data volume", "L3 data volume", "Memory data volume"});

        for(int i=0; i<(int)gName.size(); ++i)
        {
            setCounter(gName[i], metricName[i], &curr_gid, &curr_metric_id);
            gid.push_back(curr_gid);
            metric_id.push_back(curr_metric_id);
        }

        /*        free(mem_event);
                  free(l3_event);
                  free(l2_event);
                  free(event_str);*/
    }
    ECM_validate.clear();
    ECM_validate.resize(5);

    ECM_validate_data.clear();
    ECM_validate_data.resize(3);

    //copying core values since this cannot be currently measured
    ECM_validate[0]=ECM[0];
    ECM_validate[1]=ECM[1];

    for(int i=0; i<(int)gid.size(); ++i)
    {
        perfmon_setupCounters(gid[i]);
        perfmon_startCounters();
#pragma omp barrier
        stencil->data->run();
        perfmon_stopCounters();

        /*        if(i<((int)gid.size()-1))
                  {*/
        ECM_validate[i+2]= (readDataVol(i)/LUP)/bytePerCycle[gName[i].c_str()][0];
        ECM_validate_data[i] = (readDataVol(i)/LUP);
        /* }
           else
           {
           std::vector<double> freq_stat = readFreq(i);
           printf("Freq stat [in MHz] = min : %f, avg : %f, max:%f\n", freq_stat[0], freq_stat[1], freq_stat[2]);
           }*/
    }
    /*
       ECM_validate[3]= (readDataVol(1)/LUP)/bytePerCycle["L3"][0];
       ECM_validate_data[1] = (readDataVol(1)/LUP);

       ECM_validate[4]= (readDataVol(2)/LUP)/bytePerCycle["MEM"][0];
       ECM_validate_data[2] = (readDataVol(2)/LUP);
       */

}

perfModel operator+(const perfModel& a, const perfModel& b)
{
    if( ((a.stencil != b.stencil) || fabs(a.cpu_freq - b.cpu_freq) > 1e-10) || (a.portCycle.size() != b.portCycle.size()) )
    {
        ERROR_PRINT("Performance models cannot be easily concatenated due to difference in stencils; expect wrong perf. results\n");
    }

    perfModel out(a.stencil, a.cpu_freq);
    out.setDerived(true);

    //combine portCycle
    for(std::map<std::string, double>::const_iterator it=a.portCycle.begin(); it!=a.portCycle.end(); ++it)
    {
        std::string portName = it->first;
        out.portCycle[portName] = a.weight*a.portCycle.at(portName) + b.weight*b.portCycle.at(portName);
    }

    out.ECM_data.clear();
    out.ECM.clear();
    out.ECM_latency.clear();
    out.ECM_prefetch.clear();
    out.ECM_prefetch_cy.clear();
    out.ECM_boundary.clear();
    out.ECM_boundary_cy.clear();
    out.ECM_assoc.clear();
    out.ECM_assoc_cy.clear();


    //combine ECM_data, ECM_latency, ECM and ECM_penalty
    for(int i=0; i<(int)a.ECM_data.size(); ++i)
    {
        out.ECM_data.push_back(a.weight*a.ECM_data[i]+b.weight*b.ECM_data[i]);
    }
    for(int i=0; i<(int)a.ECM.size(); ++i)
    {
        out.ECM.push_back(a.weight*a.ECM[i]+b.weight*b.ECM[i]);
    }
    for(int i=0; i<(int)a.ECM_latency.size(); ++i)
    {
        out.ECM_latency.push_back(a.weight*a.ECM_latency[i]+b.weight*b.ECM_latency[i]);
    }
    for(int i=0; i<(int)a.ECM_prefetch.size(); ++i)
    {
        out.ECM_prefetch.push_back(a.weight*a.ECM_prefetch[i]+b.weight*b.ECM_prefetch[i]);
        out.ECM_prefetch_cy.push_back(a.weight*a.ECM_prefetch_cy[i]+b.weight*b.ECM_prefetch_cy[i]);
    }
    for(int i=0; i<(int)a.ECM_boundary.size(); ++i)
    {
        out.ECM_boundary.push_back(a.weight*a.ECM_boundary[i]+b.weight*b.ECM_boundary[i]);
        out.ECM_boundary_cy.push_back(a.weight*a.ECM_boundary_cy[i]+b.weight*b.ECM_boundary_cy[i]);
    }
    for(int i=0; i<(int)a.ECM_assoc.size(); ++i)
    {
        out.ECM_assoc.push_back(a.weight*a.ECM_assoc[i]+b.weight*b.ECM_assoc[i]);
        out.ECM_assoc_cy.push_back(a.weight*a.ECM_assoc_cy[i]+b.weight*b.ECM_assoc_cy[i]);
    }


    out.ECM_penalty = a.weight*a.ECM_penalty + b.weight*b.ECM_penalty;

    //weight of out is set to 1
    out.weight = 1;

    //calculate saturation such that the performance remains; this should
    //automatically result
    // 1/(a.weight/a.perf + b.weight/b.perf)
    /*double a_perf = a.getPerf();
      double b_perf = b.getPerf();

      double out_perf = 1.0/ ( (a.weight/(double)a_perf) + (b.weight/(double)b_perf) );*/
    out.chosen_mem_l3_bw = (out.ECM_data[2]/(double)out.ECM[4])*out.cpu_freq;

    return out;
}
