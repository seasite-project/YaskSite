#!/bin/bash

#Usage ./measure.sh LD_ST_bench_DIR COMPILER INSTR MIN_LD MAX_LD

ld_st_dir=$1
compiler=$2
instr=$3
min_ld=$4
max_ld=$5
ld_inc=$6

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

mc_file=$(cat $tool_dir/mc_file.txt)
nthread=$($tool_dir/threadPerNUMA.sh $mc_file)

shared_caches=""
shared_cache_idx=""
numCache=$($tool_dir/cacheInfo/numCache.sh $mc_file)
for ((idx=0; idx<$numCache; ++idx)); do
    cores_per_group=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$idx;cores per group")

    if [[ $cores_per_group -gt 1 ]]; then
        shared_cache_idx="${shared_cache_idx} $idx"
    fi
done


function measure()
{
    cache_size=$1
    cache_shared=$2
    ld=$3
    nthread=$4
    ld_st_dir=$5
    compiler=$6
    instr=$7
    threads_per_core=$8
    isVictim=$9

    if [[ "$cache_shared" == "true" ]]; then
        cache_size=$(echo $cache_size/$nthread | bc -l)
    fi

    res=$($ld_st_dir/LDST-bench INTEL_ASM:$instr $compiler $nthread $ld 1 $cache_size -c E:N:${nthread}:1:${threads_per_core})
    bw=""
    if [[ "$isVictim" == "false" ]]; then
        bw=$(echo $res | grep -o -P "(?<=Measured Bandwidth with WA \(RFO\) =).*(?=MB/s)")
    else
        bw=$(echo $res | grep -o -P "(?<=Measured Bandwidth =).*(?=Measured)" | grep -Eo '[+-]?[0-9]+([.][0-9]+)?')
        bw=$(echo "$bw*2" | bc -l)
    fi
    echo "$bw"
}


for high_idx in $shared_cache_idx; do
    low_idx=$(echo $high_idx-1 | bc -l)
    cache_size_high=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $high_idx "size")
    cache_size_low=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $low_idx "size")

    #convert sizes to MB
    cache_size_high=$(echo "0.75*$cache_size_high/(1024.0*1024.0)" | bc -l) #0.95 for CLX due to its adaptive L3 cache
    cache_size_low=$(echo "0.66*$cache_size_low/(1024.0*1024.0)" | bc -l)

    echo "cache sizes = $cache_size_low and $cache_size_high"
    cache_shared_high="true"
    cache_shared_low="false"

    cores_per_group_low=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$low_idx;cores per group")
    if [[ $cores_per_group_low -gt 1 ]]; then
        cache_shared_low="true"
    fi

    high_victim="false"
    victim_to=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$low_idx;cache per group;victims_to")
    curr_cache_name=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $high_idx "name")

    low_cache_name=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $((high_idx-1)) "name")
    echo "names = $low_cache_name $curr_cache_name"

    if [[ "$victim_to" == "$curr_cache_name" ]]; then
        high_victim="true"
    fi
    nthreads=$($tool_dir/threadPerNUMA.sh $mc_file)
    freq=$($tool_dir/getFreq.sh $mc_file)


    threads_per_core=$($tool_dir/yamlParser/yamlParser $mc_file "threads per core")
    threads_per_core=$(echo $threads_per_core)
    curr_cache_cores=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $high_idx "cores")

    high_overlap=1
    overlap_check=$(grep "overlap hypothesis.*T_${low_cache_name}.*+.*T_${curr_cache_name}" $mc_file)
    if [ ! -z "$overlap_check" -a "$overlap_check" != " " ]; then
        high_overlap=0
    fi
    echo "o/p = $high_overlap"
    echo "instr = $instr"
    for (( ld=$(($min_ld)); ld<=$max_ld; ld=${ld}+${ld_inc} )); do
        bw_res=""
        for (( thread=1; thread<=$nthreads; thread++ )); do
            active_groups_float=$(echo "($thread-1)/$curr_cache_cores" | bc -l)
            active_groups=$(echo "$active_groups_float/1" | bc)
            echo "active groups = $active_groups"
            cache_size_high_curr=$cache_size_high
            if [[ "$cache_shared_high" == "true" ]]; then
                if [[ $active_groups -ge 1 ]]; then
                    cache_size_high_curr=$(echo "$cache_size_high*($thread)/$curr_cache_cores" | bc -l)
                fi
            fi
            echo "high size = $cache_size_high_curr"
            cache_low_bw=0
            if [[ $high_overlap == 0 ]]; then
                cache_low_bw=$(measure $cache_size_low $cache_shared_low $ld $thread $ld_st_dir $compiler $instr $threads_per_core "false")
            fi
            cache_high_bw=$(measure $cache_size_high_curr $cache_shared_high $ld $thread $ld_st_dir $compiler $instr $threads_per_core "$high_victim")

            if [[ $high_overlap == 0 ]]; then
                derived_bw=$(echo "(1000.0*$freq/($cache_high_bw)) - (1000.0*$freq/($cache_low_bw))" | bc -l)
            else
                derived_bw=$(echo "(1000.0*$freq/($cache_high_bw))" | bc -l)
            fi
            derived_bw=$(echo "1/$derived_bw" | bc -l)
            derived_bw=$(echo "$derived_bw/$thread" | bc -l) #divide by thread since we need this value for ECM modelling

            if [[ $thread -eq 1 ]]; then
                bw_res="${derived_bw} B/cy"
            else
                bw_res="${bw_res},${derived_bw} B/cy"
            fi
            echo $bw_res
        done
        $tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$curr_cache_name;1;results;data path bw;$ld" -o "[$bw_res]"
    done

done
