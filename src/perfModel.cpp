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

//TODO:get cache hierarchies from hwloc
perfModel::perfModel(STENCIL* stencil_, double cpu_freq_, char* iacaOut):stencil(stencil_), cpu_freq(cpu_freq_), likwidInited(false), cpus(NULL), ECM_latency(4,0), ECM_prefetch(4,0), ECM_boundary(4,0), ECM_assoc(4,0), ECM_penalty(0)
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

        getBytePerCycles();
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

void perfModel::setReadWriteGrids(int numReadGrids_, int numWriteGrids_)
{
    numReadGrids = numReadGrids_;
    numWriteGrids = numWriteGrids_;
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

void perfModel::getBytePerCycles()
{
    if(strcmp(TARGET_ARCH,"ivb")==0 || strcmp(TARGET_ARCH,"snb")==0)
    {
        bytePerCycle["L2"] = {32};
        bytePerCycle["L3"] = {32};
    }
    else if(strcmp(TARGET_ARCH,"hsw")==0)
    {
        bytePerCycle["L2"] = {51.6};
        bytePerCycle["L3"] = {48.8};
    }
    else if(strcmp(TARGET_ARCH, "skx")==0)
    {
        bytePerCycle["L2"] = {48};
        bytePerCycle["L3"] = {32};
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

    char* bw_file;
    STRINGIFY(bw_file, "%s/bench_results/bw_load_ratio.txt", TEMP_DIR);
    bool success = readTable(bw_file, bw_table, 1, 10, 2, 2);
    if(success==false)
    {
        ERROR_PRINT("Memory bandwidth not calibrated, please go to build directory of YASKSITE and execute make calibrate");
    }
    delete[] bw_file;

    for(unsigned i=0; i<bw_table.size(); ++i)
    {
        bw_table[i]=bw_table[i]/(cpu_freq*1000.0);
    }
    bytePerCycle["MEM"] = bw_table;//18.5;
}

//Values are in words
//temporalStoreMode - 0 - read and write (default)
//                    1 - no store
//                    2 - store only
//Return {Total Data, LD:ST ratio}
std::vector<double> perfModel::getDataContrib(char* cache_str, blockDetails* opt, int temporalStoreMode)
{
    cache_info currCache = CACHE(cache_str);

    //Right now assuming only one stencil per eqn. Group
    double rest_contrib = (numReadGrids-1.0)+2*numWriteGrids;//1.0+(stencil->s-1.0);
    printf("Read Grids = %d, writeGrids = %d\n", numReadGrids, numWriteGrids);
    int totalGrids = numReadGrids+numWriteGrids;

    double prefetch_oh_per_LUP = getPrefetchEffects(currCache);
    printf("cache = %s, prefetch = %f\n", currCache.name.c_str(), prefetch_oh_per_LUP);
    //in cycles
    double latency_oh_per_LUP = getLatencyEffects(currCache);

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

        double totalSpatialContrib = (rest_contrib+1.0+extra_data+assocOh[0]);/*TODO compute per region*/
        double totalStreams = (totalGrids - 1.0)+1.0;

        if(temporalStoreMode == 1)
        {
            totalSpatialContrib-=1; //NO WA
        }
        else if(temporalStoreMode == 2)
        {
            totalSpatialContrib=1; //only store
        }

        //temporal effect only from second timestep; i.e. when the dx reduces
        //with prescribed angle
        double latency_oh_per_LUP_temp = getLatencyEffects(currCache, true)*(stencil->data->dt-1.0)/((double)stencil->data->dt);
        ECM_latency[currCache.hierarchy] = totalStreams*latency_oh_per_LUP_temp + (totalStreams)*latency_oh_per_LUP/static_cast<double>(stencil->data->dt);
        ECM_boundary[currCache.hierarchy] = spatialOh[0]*currCache.bytePerWord/static_cast<double>(stencil->data->dt);
        ECM_prefetch[currCache.hierarchy] = prefetch_oh_per_LUP*totalGrids*currCache.bytePerWord/static_cast<double>(stencil->data->dt);
        ECM_assoc[currCache.hierarchy] = assocOh[0]*currCache.bytePerWord/static_cast<double>(stencil->data->dt);
        return { ((totalSpatialContrib)/static_cast<double>(stencil->data->dt))+temporalOh[0], ((totalSpatialContrib-numWriteGrids)/(double)numWriteGrids) };
    }
    //if greater than Outer blocked LC
    else if(currCache.hierarchy > opt->spatialOBC->hierarchy)
    {
        double prefetch_oh = totalGrids*prefetch_oh_per_LUP;
        double boundary_oh = spatialOh[0];
        double extra_data = boundary_oh+prefetch_oh;
        double actual_data = rest_contrib+1.0;
        double shrink_space = 1.0 - (extra_data)/(extra_data+actual_data);
        bool ilc_jump = false, olc_jump = false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);
        double latency_oh = (totalGrids)*latency_oh_per_LUP;

        if(olc_jump)
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
            double streams = (stencil_contrib_outer + stencil_centre + totalGrids - 1);
            prefetch_oh = streams*prefetch_oh_per_LUP;
            boundary_oh = spatialOh[1];
            extra_data = prefetch_oh + boundary_oh;
            latency_oh = streams*latency_oh_per_LUP;
        }

        double totalData = actual_data + extra_data + assocOh[0];
        ECM_prefetch[currCache.hierarchy] = prefetch_oh*currCache.bytePerWord;
        ECM_boundary[currCache.hierarchy] = boundary_oh*currCache.bytePerWord;
        ECM_latency[currCache.hierarchy] = latency_oh;
        ECM_assoc[currCache.hierarchy] = assocOh[0]*currCache.bytePerWord;
        return { totalData, (totalData-numWriteGrids)/((double)numWriteGrids) };
    }
    //TODO: only for symmetric stencils currently
    //if greater than Inner blocked LC
    //stencil_data = (2*(int((rad-1)/fold) + 1) + 1)
    else if(currCache.hierarchy > opt->spatialIBC->hierarchy || (stencil->dim == 2))
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

        double prefetch_oh = (stencil_contrib_outer + stencil_centre + totalGrids - 1)*prefetch_oh_per_LUP;
        double boundary_oh = spatialOh[1];
        double extra_data = boundary_oh+prefetch_oh;
        double actual_data = stencil_contrib_outer + stencil_centre + rest_contrib;
        double shrink_space = 1.0 - (extra_data)/(extra_data+actual_data);
        bool ilc_jump = false, olc_jump = false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);
        double latency_oh = (stencil_contrib_outer + stencil_centre + totalGrids - 1)*latency_oh_per_LUP;
        if(ilc_jump)
        {
            int stencil_contrib_inner = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_y)) +1 );
            double streams = stencil_contrib_outer + stencil_contrib_inner + stencil_centre + totalGrids - 1;
            prefetch_oh = streams*prefetch_oh_per_LUP;
            boundary_oh = spatialOh[2];
            extra_data = prefetch_oh + boundary_oh;
            latency_oh = streams*latency_oh_per_LUP;
        }

        double totalData = actual_data + extra_data + assocOh[1];

        ECM_latency[currCache.hierarchy] = latency_oh;
        ECM_prefetch[currCache.hierarchy] = prefetch_oh*currCache.bytePerWord;
        ECM_boundary[currCache.hierarchy] = boundary_oh*currCache.bytePerWord;
        printf("assoc Oh = %f\n", assocOh[1]);
        ECM_assoc[currCache.hierarchy] = assocOh[1]*currCache.bytePerWord;
        return {totalData, ((totalData-numWriteGrids)/(double)numWriteGrids)};
    }
    //no blocking condition
    else
    {
        int stencil_contrib_outer = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_x)) +1 );
        int stencil_contrib_inner = 2*( static_cast<int>((stencil->radius-1)/((double)stencil->fold_y)) +1 );
        int stencil_centre = 1;

        ECM_latency[currCache.hierarchy] = (stencil_contrib_outer + stencil_contrib_inner + stencil_centre + totalGrids - 1)*latency_oh_per_LUP;
        double prefetch_oh = (stencil_contrib_outer + stencil_contrib_inner + stencil_centre + totalGrids - 1)*prefetch_oh_per_LUP;
        double extra_data = spatialOh[2]+prefetch_oh;
        double actual_data = stencil_contrib_outer + stencil_contrib_inner + stencil_centre + rest_contrib;
        double shrink_space = 1.0 - (extra_data)/(extra_data+actual_data);
        bool ilc_jump=false, olc_jump=false;
        std::vector<double> assocOh = simulateCache_assoc(ilc_jump, olc_jump, currCache, shrink_space);
        double totalData = actual_data + extra_data + assocOh[2];

        ECM_prefetch[currCache.hierarchy] = prefetch_oh*8*currCache.bytePerWord;
        ECM_boundary[currCache.hierarchy] = spatialOh[2]*currCache.bytePerWord;
        ECM_assoc[currCache.hierarchy] = assocOh[2]*currCache.bytePerWord;
        return {totalData, ((totalData-numWriteGrids)/(double)numWriteGrids)};
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

    double bx = (temporal)?stencil->data->rx:stencil->data->bx;
    double by = (temporal)?stencil->data->ry:stencil->data->by;
    double bz = (temporal)?stencil->data->rz:stencil->data->bz;

    double rx = (temporal)?stencil->data->rx:stencil->data->rx;
    double ry = (temporal)?stencil->data->ry:stencil->data->ry;
    double rz = (temporal)?stencil->data->rz:stencil->data->rz;


    double fold_x = stencil->data->fold_x;
    double fold_y = stencil->data->fold_y;
    double fold_z = stencil->data->fold_z;

    bz = bz*fold_x*fold_y;
    by = ceil(by/fold_y);
    bx = ceil(bx/fold_x);

    rz = rz*fold_x*fold_y;
    ry = ceil(ry/fold_y);
    rx = ceil(rx/fold_x);


    int num_block_z = (int)ceil(stencil->data->dz/bz);

    int dim = stencil->dim;

    if(dim!=3)
    {
        bx =1;
    }

    //get the volume of region
    double vol = bx*by*bz;

    int z_scan = std::min(num_block_z, CL_SIZE);

    double cl_avg_z = 0;

    //check for cache line effects in xy face
    for(int i=0; i<z_scan; ++i)
    {
        cl_avg_z += (static_cast<int>(bz)%CL_SIZE);
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

   //temporal blocking has radius 1
    if(temporal)
    {
        radius_x = 1;
        radius_y = 1;
        radius_z = 1;
    }

    if(dim==3)
    {
        //area of block, with its avg. data contributions
        double data_zy = face*radius_x*(bz)*(by);
        double data_zx = face*radius_y*(bz)*(bx);
        double data_xy = face*radius_z*(bx)*(by);
        //CL effect
        data_xy  += face*cl_avg_z*(bx)*(by);
        printf("cl_avg_z = %f\n", cl_avg_z);
        if(cache.shared && (radius_y+rest_contrib)*bz < cache.words)
        {
            data_xy = data_xy/thread_z;
        }
        if(cache.shared && (radius_x+rest_contrib)*bz*by < cache.words)
        {
            data_zx = data_zx/thread_y;
        }
        if(cache.shared && (rest_contrib+1)*bz*by*bx < cache.words)
        {
            data_zy = data_zy/thread_x;
        }

        //TODO: this works if and only if spatial blocking and temporal blocking are
        //near to cache size limit; if block tile is too small it might wrap
        //around reducing this effect
        //For caches greater than OLC
        wordPerLUP.push_back((data_zy+data_zx+data_xy)/vol);
        //For caches btw OLC and ILC
        wordPerLUP.push_back((data_zx+(2*radius_x+1)*data_xy)/vol);
        //For caches less than ILC
        wordPerLUP.push_back(((2*radius_x)+(2*radius_y)+1)*(data_xy)/vol);
        printf("data_xy = %f, data_zx = %f, data_zy = %f\n", data_xy, data_zx, data_zy);
    }
    else
    {
        double data_z = face*radius_y*(bz);
        double data_y = face*radius_z*(by);

        data_y += face*cl_avg_z*(by);
        if((radius_y+rest_contrib)*bz < cache.words)
        {
            data_y = data_y/thread_z;
        }
        if((1+rest_contrib)*bz*by < cache.words)
        {
            data_z = data_z/thread_y;
        }

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
#endif
    return wordPerLUP;

}

//words per LUP
double perfModel::getPrefetchEffects(cache_info currCache)
{
#ifdef MODEL_PREFETCH_EFFECT
    double bx = stencil->data->bx;
    double by = stencil->data->by;
    double bz = stencil->data->bz;

    double rx = stencil->data->rx;
    double ry = stencil->data->ry;
    double rz = stencil->data->rz;

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

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
        cache_info upperCache = (CACHES[currCache.hierarchy-1]);
        //if one thread and whole block +  it's prefetch(currently not) + single dz fits in upper cache
        //prefetched data is reused
        if(stencil->data->nthreads == 1)
        {
            if( ((((bx*by+1)*(bz)))*totalGrids) < (upperCache.words*upperCache.sf) )
            {
                //no prefetch oh
                reuseFactor = 1;
            }
        }

        //if more than one thread on z dim && upper cache is shared => no prefetching; since data will
        //be reused by neighbouring thread for num_block_z/nthreads blocks
        if(upperCache.shared == true)
        {

           //No prefetching; because of wrap around
            if( num_block_z == actualThreads )
            {
                reuseFactor = 1;
            }
            else if(2*bz*actualThreads < upperCache.words)
            {
                //for dynamic scheduling on region loop
                reuseFactor = (1- (1/(double)thread_z));
            }
        }
    }

    //TODO:Assuming 8 doubles per CL
    if(stencil->dim==3)
    {
        prefetch_oh= (1-reuseFactor)*(currCache.prefetch_cl*8) *dx*dy*(num_block_z)/fold_factor;
    }
    else
    {
        prefetch_oh= (1-reuseFactor)*(currCache.prefetch_cl*8)*dy*(num_block_z)/fold_factor;
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
double perfModel::getLatencyEffects(cache_info currCache, bool temporal)
{
#ifdef MODEL_LATENCY_EFFECT
    //for entire block overhead
    double latency_oh = 0;

    double bz = temporal ? stencil->data->rz:stencil->data->bz;

    double dx = stencil->data->dx;
    double dy = stencil->data->dy;
    double dz = stencil->data->dz;

    int num_block_z = (int)ceil(dz/(((dz-bz)<stencil->data->fold_z)?dz:bz));
    int nthreads = temporal ? 1:stencil->data->nthreads;

    double vol = dx*dy*dz;

    double fold_factor = stencil->data->fold_x*stencil->data->fold_y;

    //if num_block_z == 1 it implies no latency; since it  wraps around
    //correctly
    if((num_block_z>1 || temporal))
    {
        if(stencil->dim==3)
        {
            latency_oh=( currCache.getLatency(nthreads)*cpu_freq )*dx*dy*(num_block_z)/fold_factor;
        }
        else
        {
            latency_oh=( currCache.getLatency(nthreads)*cpu_freq )*dy*(num_block_z)/fold_factor;
        }
    }

    latency_oh = latency_oh/vol;
    printf("latency = %f, latency_oh = %f\n", currCache.getLatency(nthreads), latency_oh);
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

    double curr_cache_size = currCache.words*currCache.sf;
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
        double curr_cache_size = CACHES[i].words*CACHES[i].sf;

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
        double curr_cache_size = CACHES[i].words*CACHES[i].sf;

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

    set_size = static_cast<int>(set_size*shrink_size);

    int assoc = cache.ways;

    double centre = dx*dy*dz;
    int r = stencil->radius;
    double centre_rhs = (dx+2*r)*(dy+2*r)*(dz+2*r)+dx*dy*dz;

    //separate different words, in order to avoid counting cache hits as misses
    std::vector<double> words_nlc, words_ilc, words_olc;
    std::vector<double> words_miss_nlc, words_miss_ilc, words_miss_olc;
    std::vector<double> words_hit_nlc, words_hit_ilc, words_hit_olc;
    std::vector<double> words_drop_nlc, words_drop_ilc, words_drop_olc;

    words_miss_nlc.push_back(centre_rhs);
    words_miss_ilc.push_back(centre_rhs);
    words_miss_olc.push_back(centre_rhs);

    double radius_x = ( static_cast<int>((stencil->radius-1)/(fold_x)) + 1 );
    double centre_next = dx*dy*dz + dy*dz;
    double extreme_x = dz*dy*(radius_x)*static_cast<int>(fold_x) + centre_next;
    double min_x = -1.0*dz*dy*radius_x*static_cast<int>(fold_x) + centre;
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
    words_olc.push_back(extreme_y);
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
    double extra_cl_nlc = 0;
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

    double extra_cl_ilc =  0;
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

    for(int i=1; i<=olc_layer_cl; ++i)
    {
        for(int j=0; j<(int)cl_olc.size(); ++j)
        {
            cl_olc[j]=(cl_olc[j]+1)%set_size;
            way_ctr_olc[cl_olc[j]] += 1;
        }
    }

    double extra_cl_olc =  0;
    int ctr_olc = 0;

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

    printf("finished\n");

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
#else
    UNUSED(currCache);
    UNUSED(ilc_jump);
    UNUSED(olc_jump);
    UNUSED(shrink_size);
    return {0,0,0};
#endif
}
#endif

//Does an ECM modelling for given stencil
//returns the MEM->L3 bw chosen
void perfModel::calc_ECM(int scale, int temporalStoreMode)
{
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
    ECM_prefetch_cy.push_back(0);
    ECM_boundary_cy.push_back(0);
    ECM_assoc_cy.push_back(0);

    //L2->L1
    std::vector<double> l2_l1_data = getDataContrib("L2",&opt,temporalStoreMode);
    double l2_l1_transfer_rate = bytePerCycle["L2"][0];
    double l2_l1 = (l2_l1_data[0]*8)/l2_l1_transfer_rate;
    ECM.push_back((l2_l1/data_factor)*scale);
    ECM_data.push_back(l2_l1_data[0]*8);
    ECM_prefetch_cy.push_back(ECM_prefetch[1]/l2_l1_transfer_rate);
    ECM_boundary_cy.push_back(ECM_boundary[1]/l2_l1_transfer_rate);
    ECM_assoc_cy.push_back(ECM_assoc[1]/l2_l1_transfer_rate);


   //L3->L2
    std::vector<double> l3_l2_data = getDataContrib("L3",&opt,temporalStoreMode);
    double l3_l2_transfer_rate = bytePerCycle["L3"][0];
    double l3_l2 = (l3_l2_data[0]*8)/l3_l2_transfer_rate;
    ECM.push_back((l3_l2/data_factor)*scale);
    ECM_data.push_back(l3_l2_data[0]*8);
    ECM_prefetch_cy.push_back(ECM_prefetch[2]/l3_l2_transfer_rate);
    ECM_boundary_cy.push_back(ECM_boundary[2]/l3_l2_transfer_rate);
    ECM_assoc_cy.push_back(ECM_assoc[2]/l3_l2_transfer_rate);


    //MEM->L3
    std::vector<double> mem_l3_data = getDataContrib("MEM",&opt,temporalStoreMode);
    int bw_idx = (int)(round(mem_l3_data[1]))-1;
    bw_idx = (bw_idx<0)?0:bw_idx;
    //restrict to ratio 9
    bw_idx = std::min(9,bw_idx);
    double mem_l3_transfer_rate = bytePerCycle["MEM"][bw_idx];
    double mem_l3 = (mem_l3_data[0]*8)/mem_l3_transfer_rate;
    ECM.push_back((mem_l3/data_factor)*scale);
    ECM_data.push_back(mem_l3_data[0]*8);
    ECM_prefetch_cy.push_back(ECM_prefetch[3]/mem_l3_transfer_rate);
    ECM_boundary_cy.push_back(ECM_boundary[3]/mem_l3_transfer_rate);
    ECM_assoc_cy.push_back(ECM_assoc[3]/mem_l3_transfer_rate);

    chosen_mem_l3_bw = (bytePerCycle["MEM"][bw_idx]*cpu_freq);
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
            double burst_sat = getSaturation();
            double burst_cy = applyNOL();

            //now model in-cache phase
            stencil->data->dt = 1000000; //making a big dt to simulate 0  memory transfer
            calc_ECM(scale);
            double inCache_sat = getSaturation();
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
            double sat = getSaturation();
            double cy  = applyNOL();

            /*printf("burst: cy = %f sat = %f\n", burst_cy, burst_sat);
              printf("inCache: cy = %f sat = %f\n", inCache_cy, inCache_sat);
              printf("even: cy = %f sat = %f\n", cy, sat);*/
            //calculate penalty
            //double actual_cy = sat*(1.0*burst_cy/burst_sat + (orig_dt-2.0)*inCache_cy/inCache_sat + 1.0*store_cy/store_sat)/((double)orig_dt);
            double actual_cy = sat*(1.0*burst_cy/burst_sat + (orig_dt-1.0)*inCache_cy/inCache_sat)/((double)orig_dt);
            ECM_penalty = (actual_cy - cy);
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

    double perf=applyNOL();
    double sat = getSaturation();

    printf("Saturation @ %5.2f\n",sat);
    //convert cy/LUP to MLUP/s
    double mlups = sat*(cpu_freq/perf)*1000.0;
    printf("ECM Perf = %f MLUP/s\n\n", mlups);

    if(val)
    {
        printf("Validation\n");
        //no penalty in validation
        printECM_style(ECM_validate, ECM_validate_data, 0);
    }

    printf("\n\n");
}

//To fix by splitting nthreads_x, nthreads_y, nthreads_z
//i.e. reminder in y can be divided by nthreads_z threads
double perfModel::getSaturation()
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
        sat = totalNOL/mem_l3;
        sat = std::min(sat,effThreads);
    }
    return sat;
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
    double sat = getSaturation();
    double mlups = sat*(cpu_freq/perf)*1000.0;
    return mlups;
}

double perfModel::readDataVol(int eOffsetId)
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

        gid = perfmon_addEventSet(event_str);
        perfmon_setupCounters(gid);

        free(mem_event);
        free(l3_event);
        free(l2_event);
        free(event_str);
    }

    ECM_validate.clear();
    ECM_validate.resize(5);

    ECM_validate_data.clear();
    ECM_validate_data.resize(3);

    //copying core values since this cannot be currently measured
    ECM_validate[0]=ECM[0];
    ECM_validate[1]=ECM[1];

    perfmon_startCounters();
#pragma omp barrier
    stencil->data->run();
    perfmon_stopCounters();

    ECM_validate[2]= (readDataVol(0)/LUP)/bytePerCycle["L2"][0];
    ECM_validate_data[0] = (readDataVol(0)/LUP);

    ECM_validate[3]= (readDataVol(1)/LUP)/bytePerCycle["L3"][0];
    ECM_validate_data[1] = (readDataVol(1)/LUP);

    ECM_validate[4]= (readDataVol(2)/LUP)/bytePerCycle["MEM"][0];
    ECM_validate_data[2] = (readDataVol(2)/LUP);

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
