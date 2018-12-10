#!/bin/bash

#updates only if the file does not exist or does not comply to machine
#USAGE: ./latency_scan.sh TOOL_DIR TEMP_DIR LIC_FILE SIZE(KB) THREAD MEM_TYPE OUT_FILE

tool_dir=$1
temp_dir=$2
lic_file=$3
SIZE=$4
nthreads=$5
mem_type=$6
out_file=$7

cacheStale=$($tool_dir/bw_bench/needCalibration.sh $out_file)

if [[ $nthreads == -1 ]]; then
    if [[ $mem_type = *"private"* ]]; then
        nthreads=1
    else
        nthreads=$($tool_dir/threadPerSocket.sh)
    fi
fi

mkdir -p $temp_dir

lic_agree=$(cat $lic_file 2>/dev/null)

if [[ $cacheStale != "0" ]]; then
    printf "%10s %12s %12s\n" "THREADS" "Latency(ns)" "SIZE(KB)" > $out_file

    if [[ $lic_agree == "1" ]]; then
        echo "Doing latency measurements for size=$SIZE KB; this could take 1-2 min."
        curr_dir=$PWD
        cd $tool_dir/MLC/mlc
        make clean
        make

        ./mlc_no_pf -b $SIZE -t 1-1 -d 0 > $temp_dir/latency_out.txt #no noise threads not supported
        lat=$(grep -A2 "Delay.*ns" $temp_dir/latency_out.txt | tail -n 1 | awk '{print $2}'| grep -Eo '[0-9]*\.?[0-9]+')
        printf "%10s %12s %12s\n" 1 $lat $SIZE >> $out_file

        for (( thread=2; thread<=$nthreads; thread++ )); do
            ./mlc_no_pf -b $SIZE -t 1-$((thread-1)) -d 0 > $temp_dir/latency_out.txt
            lat=$(grep -A2 "Delay.*ns" $temp_dir/latency_out.txt | tail -n 1 | awk '{print $2}'| grep -Eo '[0-9]*\.?[0-9]+')

            printf "%10s %12s %12s\n" $thread $lat $SIZE >> $out_file
        done

        cd $curr_dir
    else
        for (( thread=1; thread<=$nthreads; thread++ )); do
            printf "%10s %12s %12s\n" $thread "0.0" $SIZE >> $out_file
        done
    fi

    printf "\nMachine Info\n" >> $out_file
    lscpu | grep "Model name:" >> $out_file
    lscpu | grep "CPU MHz:" >> $out_file
fi
