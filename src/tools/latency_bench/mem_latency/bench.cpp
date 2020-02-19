#include <stdio.h>
#include <stdlib.h>
#include "dummy.h"
#include "timing.h"
#include "omp.h"
#include "likwid.h"
#include <string>
#include <vector>
#include <omp.h>

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
            break;
        }
    }
    return std::vector<int>{gid,dataVol_metric_id};
}

#ifdef STREAMS

#define LOOP_with_write(_repeat_)\
    lup=0;\
    jump=0;\
    tmp=0;\
    for(int r=0; r<_repeat_; ++r)\
    {\
        for(int block=0; block<size_wo_streams; block+=(skip_blocks_priv+block_size_priv))\
        {\
            ++jump;\
            for(int i=block; i<(block+block_size_priv); ++i)\
            {\
                ++lup;\
                for(int stream=1; stream<STREAMS; ++stream)\
                {\
                    dataPtr[0][i] += (dataPtr[stream][i]);\
                }\
            }\
        }\
        dummy(data, data, &tmp);\
    }


#define LOOP(_repeat_)\
    lup=0;\
    jump=0;\
    tmp=0;\
    for(int r=0; r<_repeat_; ++r)\
    {\
        for(int block=0; block<size_wo_streams; block+=(skip_blocks_priv+block_size_priv))\
        {\
            ++jump;\
            for(int i=block; i<(block+block_size_priv); ++i)\
            {\
                ++lup;\
                for(int stream=0; stream<STREAMS; ++stream)\
                {\
                    tmp += (dataPtr[stream][i]);\
                }\
            }\
        }\
        dummy(data, data, &tmp);\
    }

#else

#define LOOP_with_write(_repeat_)\
    lup=0;\
    jump=0;\
    tmp=0;\
    for(int r=0; r<_repeat_; ++r)\
    {\
        for(int block=0; block<size_wo_streams; block+=(skip_blocks_priv+block_size_priv))\
        {\
            ++jump;\
            for(int i=block; i<(block+block_size_priv); ++i)\
            {\
                ++lup;\
                for(int stream=1; stream<streams; ++stream)\
                {\
                    dataPtr[0][i] += (dataPtr[stream][i]);\
                }\
            }\
        }\
        dummy(data, data, &tmp);\
    }


#define LOOP(_repeat_)\
    lup=0;\
    jump=0;\
    tmp=0;\
    for(int r=0; r<_repeat_; ++r)\
    {\
        for(int block=0; block<size_wo_streams; block+=(skip_blocks_priv+block_size_priv))\
        {\
            ++jump;\
            for(int i=block; i<(block+block_size_priv); ++i)\
            {\
                ++lup;\
                for(int stream=0; stream<streams; ++stream)\
                {\
                    tmp += (dataPtr[stream][i]);\
                }\
            }\
        }\
        dummy(data, data, &tmp);\
    }

#endif

//This includes benchmarks to measure bandwidth depending on LD:ST ratios
//return bw in GB/s
//minimum ratio is 1; i.e 1 load and 1 store; and load >= store
//double bench(double cache_hi_, double cache_lo_, int streams, int repeat)
double bench(double low_cache_size, int streams, long int repeat)
{
#ifdef STREAMS
    if(streams != STREAMS)
    {
        printf("Error: compiled streams value does not match, if you wan't general version don't compile with STREAM value\n");
        exit(-1);
    }
    streams = STREAMS;
#endif
#ifdef WRITE_LOOP
    if(streams == 1)
    {
        printf("Error : Atleast 2 streams required if write is enabled, using 2 streams\n");
        exit(-1);
    }
#endif

    printf("streams = %d, repeat = %ld\n", streams, repeat);

    //it's assumed atleast 1GB main memory is present
    double high_cache_size = 1e9;
    //MEM
    //double high_cache_size = 1*1e9;
    //double low_cache_size = 30*1e6;

    //L3
    //double high_cache_size = 20*1e6;
    //double low_cache_size = 300*1e3;

    //L2
    //double high_cache_size = 200*1e3;
    //double low_cache_size = 50*1e3;

    //double high_cache_size = cache_hi_;
    //double low_cache_size = cache_lo_;


    double size = static_cast<int>(low_cache_size/(8.0));
    int block_size = 20*8;//20CL
    int skip_factor = 16; //std::min(16,static_cast<int>(high_cache_size/low_cache_size));
    int skip_blocks = block_size*skip_factor;
    printf("skip factor = %d\n", skip_factor);
   // size*=block_size; //multiplied by skip blocks to get from memory
   // multiple of block_size only since this is min data, max data we don't care
   // for latency
    int multiple = static_cast<int>(static_cast<double>(size)/static_cast<double>(streams*block_size));//(skip_blocks+block_size)));
    size = static_cast<double>(multiple)*streams*static_cast<double>(skip_blocks+block_size);

    printf("size = %f\n",size);
    double min_data = (size/(block_size+skip_blocks))*block_size*8;
    double max_data = size*8;
/*
    while(min_data < low_cache_size)
    {
        size *= 2;
        printf("increasing size\n");
        min_data = (size/(block_size+skip_blocks))*block_size*8;
        max_data = size*8;
    }*/
/*
    while(max_data > low_cache_size)
    {
        size /= 2;
        printf("lowering size\n");
        min_data = (size/(block_size+skip_blocks))*block_size*8;
        max_data = size*8;
    }
*/

    double num_jumps = size/((double)(streams*(block_size+skip_blocks)));
    //atleast 40 jumps compared to 1 of normal
    while(num_jumps < 40)
    {
        printf("lowering skip distance %d and blocksize %d, num jumps %f\n",skip_blocks, block_size, num_jumps);
        //skip_blocks/=2;
        block_size/=2;
        skip_blocks/=2;
        num_jumps = size/((double)(streams*(block_size+skip_blocks)));
    }

    printf("skip distance %d, blocksize %d, num jumps %f\n", skip_blocks, block_size, num_jumps);

    min_data = (size/(block_size+skip_blocks))*block_size*8;
    max_data = size*8;

    printf("data estimation: [%f, %f] MB\n", min_data*1e-6, max_data*1e-6);

#pragma omp parallel
    {
        //we need ST times ld+store streams and (LD-ST) times pure load streams
        double* data = (double*) malloc(size*sizeof(double));//*streams);
        double** dataPtr = (double**) malloc(streams*sizeof(double*));

        int size_wo_streams = (int)(size/(double)streams);

        //init data
        for(int i=0; i<size; ++i)
        {
            data[i] = 1;;
        }


        for(int i=0; i<streams; ++i)
        {
            dataPtr[i] = &(data[i*size_wo_streams]);
        }

        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        int skip_blocks_priv = skip_blocks;
        int block_size_priv = block_size;

        //printf("Tid %d\n", tid);
        if(tid != 0)
        {
            /*
            skip_blocks_priv = size_wo_streams*2;
            block_size_priv = size_wo_streams;*/

            size_wo_streams = size_wo_streams*min_data/(double)max_data;
            skip_blocks_priv = size_wo_streams*2;
            block_size_priv = size_wo_streams;
        }

        //printf("skip = %d, size = %d\n", skip_blocks_priv, block_size_priv);
        double tmp=0;
        long int warm_up = 20;
        long int lup = 0;
        long int jump = 0;

        timer_init();

#pragma omp barrier
        TimerData* repeatTime = new TimerData;
        double totTime = 0;
        long int totIter = 0;
        while(totTime < 0.2)
        {
            timer_start(repeatTime);

#ifdef WRITE_LOOP
            LOOP_with_write(warm_up);
#else
            //warm-up
            LOOP(warm_up);
#endif
            timer_stop(repeatTime);
            totTime += timer_print(repeatTime);
            totIter++;
        }
        long int repeat_priv = repeat;
        if(repeat_priv == -1)
        {
            repeat_priv = 1.2*warm_up*totIter/totTime;
        }

#pragma omp barrier
        if(tid!=0)
        {
            repeat_priv = static_cast<int>(1.2*repeat_priv);
        }
        else
        {
            printf("Iter chosen as %ld\n", repeat_priv);
            repeat = 2*repeat_priv;
        }

        //printf("tid = %d, repeat = %d\n", omp_get_thread_num(), repeat_priv);
        double runtime_with_hole = 0;
        //    std::vector<int> runtime_ctr = set_counter("CLOCK", "Runtime");

//just warming up
#ifdef WRITE_LOOP
        LOOP_with_write(repeat_priv);
#else
        LOOP(repeat_priv);
#endif

       tmp = 0;

        TimerData* time_with_hole = new TimerData;

#pragma omp barrier
        //create benchmark
        if(tid == 0)
        {
            timer_start(time_with_hole);
        }

#ifdef WRITE_LOOP
        LOOP_with_write(repeat_priv);
#else
        LOOP(repeat_priv);
#endif


        long int lup_with_hole, jump_with_hole;

        if(tid==0)
        {
            timer_stop(time_with_hole);

            runtime_with_hole = timer_print(time_with_hole);//(perfmon_getLastMetric(runtime_ctr[0], runtime_ctr[1], thread_id));
            lup_with_hole = lup;
            jump_with_hole = jump;
            printf("%lu %lu %f\n", lup_with_hole, jump_with_hole, runtime_with_hole);
            printf("Data touched per iter = %f MB\n", lup_with_hole*8*streams*1e-6/(double)(repeat_priv));
        }

        delete time_with_hole;
        double runtime_wo_hole;
        //  std::vector<int> runtime_ctr_wo_hole = set_counter("CLOCK", "Runtime");
        //run benchmark wo holes
        size_wo_streams = size_wo_streams*min_data/(double)max_data;
        skip_blocks_priv = size_wo_streams*2;
        block_size_priv = size_wo_streams;
        repeat_priv = repeat;
        TimerData* time_wo_hole = new TimerData;

#pragma omp barrier
        if(tid == 0)
        {
            timer_start(time_wo_hole);
        }

#ifdef WRITE_LOOP
        LOOP_with_write(repeat_priv);
#else
        LOOP(repeat_priv);
#endif

        if(tid == 0)
        {
            timer_stop(time_wo_hole);

            runtime_wo_hole = timer_print(time_wo_hole);//(perfmon_getLastMetric(runtime_ctr_wo_hole[0], runtime_ctr_wo_hole[1], thread_id));
            long int lup_wo_hole = lup;
            long int jump_wo_hole = jump;

            printf("%lu %lu %f\n", lup_wo_hole, jump_wo_hole, runtime_wo_hole);
            printf("Data touched per iter = %f MB\n", lup_wo_hole*8*streams*1e-6/(double)(repeat_priv));
            printf("runtime per lup = %f ns\n", 1e9*runtime_wo_hole/(double)(lup_wo_hole));
            double extra_time_due_to_jumps = runtime_with_hole - (runtime_wo_hole*lup_with_hole/(double)lup_wo_hole);
            double extra_time_per_jump = extra_time_due_to_jumps/(double)(jump_with_hole-jump_wo_hole);
            printf("latency oh = %f ns\n", 1e9*extra_time_per_jump);
            //    double clock_cycle_per_jump = multiple*
        }
        delete time_wo_hole;
        free(data);
        free(dataPtr);
    }
    return 0;
}

int main(int argc, char** argv)
{
    cpuFeatures_init();

    int stream = 1;
    double sf = 2;
#ifdef STREAMS
    stream = STREAMS;
#endif
    int repeat  = -1;
    double mem;
    if(argc < 2)
    {
        printf("Insufficient arguments execute as: %s data_set_size(in bytes) safety_factor(optional) num_streams(optional) repeat(optional)\n", argv[0]);
        exit(0);
    }
    else
    {
        mem = atof(argv[1]);
        if(argc >= 3)
        {
            sf = atof(argv[2]);
            if(argc >= 4)
            {
                stream = atoi(argv[3]);
                if(argc >= 5)
                {
                    repeat = atoi(argv[4]);
                }
            }
        }
    }

    mem=mem/(double)sf;
    printf("data_set=%f, stream=%d, repeat=%d\n", mem, stream, repeat);

#pragma omp parallel
    {
#pragma omp master
        {
            printf("num threads = %d\n", omp_get_num_threads());
        }
    }


    bench(mem, stream, repeat);

    return 0;
}
