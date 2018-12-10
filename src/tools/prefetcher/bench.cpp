#include <stdio.h>
#include <stdlib.h>
#include "dummy.h"
#include "timing.h"
#include "omp.h"
#include "likwid.h"
#include <string>
#include <vector>

#define DISABLE_PREFETCHER \
    printf("Disabling prefetcher\n");\
    cpuFeatures_disable(0, FEAT_HW_PREFETCHER, 0); \
    cpuFeatures_disable(0, FEAT_CL_PREFETCHER, 0); \
    cpuFeatures_disable(0, FEAT_DCU_PREFETCHER, 0); \
    cpuFeatures_disable(0, FEAT_IP_PREFETCHER, 0);

#define ENABLE_PREFETCHER \
    printf("Enabling prefetcher\n");\
    cpuFeatures_enable(0, FEAT_HW_PREFETCHER, 0); \
    cpuFeatures_enable(0, FEAT_CL_PREFETCHER, 0); \
    cpuFeatures_enable(0, FEAT_DCU_PREFETCHER, 0); \
    cpuFeatures_enable(0, FEAT_IP_PREFETCHER, 0);

std::vector<int> set_counter(char* ctr_name, char* metric_string)
{
    int gid = perfmon_addEventSet(ctr_name);
    perfmon_setupCounters(gid);
    int numMetrics = perfmon_getNumberOfMetrics(gid);
    std::string toFind(metric_string);
    int dataVol_metric_id = numMetrics-1;
    for(int i=0; i<numMetrics; ++i)
    {
        std::string currMetric(perfmon_getMetricName(gid,i));
        if(currMetric.find(toFind) != std::string::npos)
        {
            dataVol_metric_id = i;
        }
    }
    return std::vector<int>{gid,dataVol_metric_id};
}


//This includes benchmarks to measure bandwidth depending on LD:ST ratios
//return bw in GB/s
//minimum ratio is 1; i.e 1 load and 1 store; and load >= store
std::vector<double> bench( int repeat)
{
    cpuFeatures_init();
    topology_init();
    CpuTopology_t topo = get_cpuTopology();
    affinity_init();

    int nthreads=1;
    int *cpus = (int*)malloc(nthreads * sizeof(int));

    for(int i=0;i<nthreads;i++)
    {
        cpus[i] = topo->threadPool[i].apicId;
    }

    perfmon_init(nthreads, cpus);

    double size = static_cast<int>(1000000/(8.0));
    int block_size = 20*8;//20CL
    int skip_blocks = block_size*4;
    size*=block_size; //multiplied by skip blocks to get from memory
    int multiple = static_cast<int>(static_cast<double>(size)/static_cast<double>(skip_blocks)+1);
    size = static_cast<double>(multiple)*static_cast<double>(skip_blocks);
    //make size multiple of block_size*skip_blocks

    printf("size = %f\n",size);
    //we need ST times ld+store streams and (LD-ST) times pure load streams
    double* a = (double*) malloc(size*sizeof(double));
    double* b = (double*) malloc(size*sizeof(double));

    //init ptrs
    for(int i=0; i<size; ++i)
    {
        a[i] = 1;;
    }

    for(int i=0; i<size; ++i)
    {
        b[i] = 1;
    }

    double tmp=0;
    int warm_up = 20;
    //warm-up
    for(int r=0; r<warm_up; ++r)
    {
        for(int block=0; block<size; block+=(skip_blocks))
        {
            for(int i=block; i<(block+block_size); ++i)
            {
                tmp += a[i] + b[i];
            }
        }

        dummy(a, b, &tmp);
    }

    std::vector<double> data_vol;

    std::vector<int> mem_ctr = set_counter("MEM", "Memory data volume");
    tmp = 0;
    perfmon_startCounters();
    LIKWID_MARKER_INIT;
    LIKWID_MARKER_START("bench");
    //create benchmark
    for(int r=0; r<repeat; ++r)
    {
        for(int block=0; block<size; block+=(skip_blocks))
        {
            for(int i=block; i<(block+block_size); ++i)
            {
                tmp += b[i] + a[i];
            }
        }

        dummy(a, b, &tmp);
    }
    perfmon_stopCounters();
    data_vol.push_back(perfmon_getLastMetric(mem_ctr[0],mem_ctr[1],0));

    std::vector<int> l3_ctr = set_counter("L3", "L3 data volume");
    tmp = 0;
    perfmon_startCounters();
    //create benchmark
    for(int r=0; r<repeat; ++r)
    {
        for(int block=0; block<size; block+=(skip_blocks))
        {
            for(int i=block; i<(block+block_size); ++i)
            {
                tmp += b[i] + a[i];
            }
        }

        dummy(a, b, &tmp);
    }
    perfmon_stopCounters();
    data_vol.push_back(perfmon_getLastMetric(l3_ctr[0],l3_ctr[1],0));

    std::vector<int> l2_ctr = set_counter("L2", "L2 data volume");
    tmp = 0;
    perfmon_startCounters();
    //create benchmark
    for(int r=0; r<repeat; ++r)
    {
        for(int block=0; block<size; block+=(skip_blocks))
        {
            for(int i=block; i<(block+block_size); ++i)
            {
                tmp += b[i] + a[i];
            }
        }

        dummy(a, b, &tmp);
    }
    perfmon_stopCounters();
    data_vol.push_back(perfmon_getLastMetric(l2_ctr[0],l2_ctr[1],0));

    printf("multiple = %d\n", multiple);
    printf("Ideal data transfer = %f GB\n", 2.0*static_cast<double>(repeat)*static_cast<double>(multiple*block_size)*8.0/(1024.0*1024.0*1024.0));
    printf("Measured data transfer : MEM->L3 = %f GB, L3->L2 = %f GB, L2->L1 = %f GB\n", data_vol[0], data_vol[1], data_vol[2]);

    //convert data_vol to CL per block_size
    for(int i=0; i<data_vol.size(); ++i)
    {
        data_vol[i] = data_vol[i]*1024.0*1024.0*1024.0/(2*repeat*multiple*8*8);
    }

    free(a);
    free(b);

    return data_vol;
}

int main(int argc, char** argv)
{
    int repeat    = 20;
    if(argc > 2)
    {
        repeat    = atoi(argv[1]);
    }


    std::vector<double> data_vol_pre = bench(repeat);
    DISABLE_PREFETCHER;
    std::vector<double> data_vol = bench(repeat);
    ENABLE_PREFETCHER;

    printf("Prefetch CL: MEM->L3 = %f\n", data_vol_pre[0]-data_vol[0]);
    printf("Prefetch CL: L3->L2 = %f\n", data_vol_pre[1]-data_vol[1]);
    printf("Prefetch CL: L2->L1 = %f\n", data_vol_pre[2]-data_vol[2]);

    return 0;
}
