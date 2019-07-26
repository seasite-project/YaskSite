#!/bin/bash

#updates only if the file does not exist or does not comply to machine
#USAGE: ./latency_scan.sh TEMP_DIR LIC_FILE OUT_FOLDER

LANG=en_US.UTF-8
tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

temp_dir=$1
lic_file=$2
#SIZE=$4
#nthreads=$5
#mem_type=$6
out_folder=$3

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

    lic_agree=$(cat $lic_file 2>/dev/null)

    if [[ $cacheStale != "0" ]]; then
        printf "%10s %12s %12s\n" "THREADS" "Latency(ns)" "SIZE(KB)" > $out_file

        lat_out=""
        if [[ $lic_agree == "1" ]]; then
            if [[ $old == "0" ]]
            then
                SIZE=$($tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;size per thread;1")
                SIZE=$($tool_dir/unitconvert.sh "$SIZE")
                SIZE=$(echo "$SIZE/(1024)" | bc -l) #convert to KB
            fi
            echo "Doing latency measurements for size=$SIZE KB; this could take 1-2 min."
            curr_dir=$PWD
            cd $tool_dir/MLC/mlc

            make clean
            make

            ./mlc_no_pf -b $SIZE -t 1-1 -d 0 > $temp_dir/latency_out.txt #no noise threads not supported
            lat=$(grep -A2 "Delay.*ns" $temp_dir/latency_out.txt | tail -n 1 | awk '{print $2}'| grep -Eo '[0-9]*\.?[0-9]+')
            printf "%10s %12s %12s\n" 1 $lat $SIZE >> $out_file
            lat_out="$lat cy"

            for (( thread=2; thread<=$nthreads; thread++ )); do
                if [[ $old == "0" ]]
                then
                    SIZE=$($tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;size per thread;$((thread-1))")
                    SIZE=$($tool_dir/unitconvert.sh "$SIZE")
                    SIZE=$(echo "$SIZE/(1024)" | bc -l) #convert to KB
                fi
                ./mlc_no_pf -b $SIZE -t 1-$((thread-1)) -d 0 > $temp_dir/latency_out.txt
                lat=$(grep -A2 "Delay.*ns" $temp_dir/latency_out.txt | tail -n 1 | awk '{print $2}'| grep -Eo '[0-9]*\.?[0-9]+')

                printf "%10s %12s %12s\n" $thread $lat $SIZE >> $out_file
                lat_out="$lat_out, $lat cy"
            done

            cd $curr_dir
        else
            printf "%10s %12s %12s\n" 1 "0.0" $SIZE >> $out_file
            lat_out="0"
            for (( thread=2; thread<=$nthreads; thread++ )); do
                printf "%10s %12s %12s\n" $thread "0.0" $SIZE >> $out_file
                lat_out="$lat_out, 0 cy"
            done
        fi

        $tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;results;latency" -o "[$lat_out]"

        printf "\nMachine Info\n" >> $out_file
        lscpu | grep "Model name:" >> $out_file
        lscpu | grep "CPU MHz:" >> $out_file
    fi

done
