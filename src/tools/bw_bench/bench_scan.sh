#!/bin/bash

#KEEPS ST=1 and scans LD from MIN_LD-MAX_LD
#updates only if the file does not exist or does not comply to machine
#USAGE: ./bench_scan.sh TOOL_DIR TEMP_DIR MIN_LD MAX_LD SIZE OUT_FILE

tool_dir=$1
temp_dir=$2
min_ld=$3
max_ld=$4
size=$5
out_file=$6

stable_freq=$(taskset -c 0-0 $tool_dir/bw_bench/cpuFreqCheck.sh)

if [[ "$stable_freq" == "0" ]]; then
    echo "Warning clock speed is not fixed"
fi

cacheStale=$($tool_dir/bw_bench/needCalibration.sh $out_file)

if [[ $cacheStale != "0" ]]; then
    echo "Doing bandwidth measurements this could take 1-2 min."

    printf "%10s %6s %12s\n" "Size(MB)" "LD:ST" "BW(MB/s)" > $out_file

    for (( ld=min_ld; ld<=max_ld; ld++ )); do
        output=$($tool_dir/bw_bench/bench.sh "$tool_dir" "$temp_dir" -1 $ld 1 $size)
        bw=$(echo $output | grep -o -P "(?<=Measured bandwidth =).*(?=MB/s)")
        printf "%10s %6s %12s\n" $size $ld $bw >> $out_file
    done

    printf "\nMachine Info\n" >> $out_file
    lscpu | grep "Model name:" >> $out_file
    lscpu | grep "CPU MHz:" >> $out_file
fi
