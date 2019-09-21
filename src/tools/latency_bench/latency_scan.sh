#!/bin/bash

#updates only if the file does not exist or does not comply to machine
#USAGE: ./latency_scan.sh TEMP_DIR OUT_FOLDER

LANG=en_US.UTF-8
tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

temp_dir=$1
#SIZE=$4
#nthreads=$5
#mem_type=$6
out_folder=$2

mkdir -p $out_folder

mc_file=$(cat $tool_dir/mc_file.txt)
numCache=$($tool_dir/cacheInfo/numCache.sh $mc_file)

for((cache_id=0; cache_id<=$numCache; cache_id+=1)); do
    #memory
    out_file="$out_folder/$cache_id"
    cacheStale=$($tool_dir/bw_bench/needCalibration.sh $out_file)
    cacheName=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache_id "name")
    SIZE=0
    old=0

    if [[ $old == "1" ]]
    then
        if [[ $cache_id == $numCache ]]
        then
            currCores=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache_id "cores")
            nthreads=$currCores
            prevSize=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $(($cache_id-1)))
            SIZE=$(echo "$prevSize+10" | bc -l)
            SIZE=$(echo $SIZE/1024 | bc -l)
        else
            currSize=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache_id)
            currCores=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache_id "cores")
            currSize=$(echo "$currSize/$currCores" | bc -l)
            nthreads=$currCores

            if [[ $cache_id == 0 ]]
            then
                SIZE=$currSize
            else
                prevSize=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $(($cache_id-1)))
                prevCores=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $(($cache_id-1)) "cores")
                prevSize=$(echo "$prevSize/$prevCores" | bc -l)
                SIZE=$(echo "$currSize+$prevSize" | bc -l)
            fi

            SIZE=$(echo $SIZE/2048 | bc -l)
        fi
    fi

    nthreads=$($tool_dir/threadPerSocket.sh $mc_file) #always scanning for full cores

    #    if [[ $nthreads == -1 ]]; then
#        if [[ $mem_type = *"private"* ]]; then
#            nthreads=1
#        else
#            nthreads=$($tool_dir/threadPerSocket.sh)
#        fi
#    fi

    mkdir -p $temp_dir

    threads_per_core=$($tool_dir/yamlParser/yamlParser $mc_file "threads per core")
    threads_per_core=$(echo $threads_per_core)

    cd $tool_dir/latency_bench/mem_latency


    if [[ $cacheStale != "0" ]]; then
        SIZE=$($tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;size per thread;0")
        SIZE=$($tool_dir/unitconvert.sh "$SIZE")
        echo "Doing latency measurements for size=$SIZE KB; this could take 1-2 min."

        factor=1

        #cacheName=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache_id "name")
        #echo $cacheName
        #TODO do this in a more nicer way such that CoD is also taken into account
        #if [[ $cacheName == "L3" ]]; then
        #    numa_per_socket=$($tool_dir/yamlParser/yamlParser $mc_file "NUMA domains per socket")
        #    if [ $numa_per_socket -ge 2 ]; then
        #        l3_size=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache_id)
        #        l3_size=$(echo "$l3_size/2" | bc -l)
        #        echo "$SIZE and $l3_size"
        #        currFactor=$(echo "1.15*$SIZE/$l3_size" | bc -l)
        #        if (( $(echo "$currFactor > 1" |bc -l) )); then
        #            factor=$currFactor
        #        fi
        #    fi
        #fi

        #echo "factor = $factor, size = $SIZE"
        readWrite="1 2 3 4 5 6"
        for rw in $readWrite; do
            lat_out=""
            ./memLatency.sh 1 $(($rw+1)) $SIZE $factor 1 E:N:1:1:$threads_per_core > $temp_dir/latency_out.txt
            lat=$(grep "latency oh" $temp_dir/latency_out.txt | cut -d"=" -f2)
            lat=$(echo $lat)
            echo $lat
            lat_out="$lat"

            for (( thread=2; thread<=$nthreads; thread++ )); do
                SIZE=$($tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;size per thread;$((thread-1))")
                SIZE=$($tool_dir/unitconvert.sh "$SIZE")
                ./memLatency.sh 1 $(($rw+1)) $SIZE $factor $thread E:N:$thread:1:$threads_per_core > $temp_dir/latency_out.txt
                lat=$(grep "latency oh" $temp_dir/latency_out.txt | cut -d"=" -f2)
                lat=$(echo $lat)
                echo $lat
                lat_out="$lat_out, $lat"
            done

            $tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;results;latency;$rw" -o "[$lat_out]"

        done
        #printf "\nMachine Info\n" >> $out_file
        #lscpu | grep "Model name:" >> $out_file
        #lscpu | grep "CPU MHz:" >> $out_file
    fi

    cd -
done
