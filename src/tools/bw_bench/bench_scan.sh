#!/bin/bash

#KEEPS ST=1 and scans LD from MIN_LD-MAX_LD
#updates only if the file does not exist or does not comply to machine
#USAGE: ./bench_scan.sh LD_ST_bench_DIR COMPILER TOOL_DIR MIN_LD MAX_LD SIZE OUT_FILE

ld_st_dir=$1
compiler=$2
tool_dir=$3
min_ld=$4
max_ld=$5
size=$6
out_file=$7

stable_freq=$(taskset -c 0-0 $tool_dir/bw_bench/cpuFreqCheck.sh)

if [[ "$stable_freq" == "0" ]]; then
    echo "Warning clock speed is not fixed"
fi

cacheStale=$($tool_dir/bw_bench/needCalibration.sh $out_file)

nthread=$($tool_dir/threadPerSocket.sh)

if [[ $cacheStale != "0" ]]; then
    echo "Doing bandwidth measurements this could take 1-2 min."

    printf "%10s %6s %12s\n" "Size(MB)" "LD:ST" "BW(MB/s)" > $out_file

    for (( ld=min_ld; ld<=max_ld; ld++ )); do
        #output=$($tool_dir/bw_bench/bench.sh "$tool_dir" "$temp_dir" -1 $ld 1 $size)
        #Using C version, since we need only full socket bandwidths
        output=$($ld_st_dir/LDST-bench C $compiler $nthread $ld 1 $size -c S0:0-$((nthread-1)))
        bw=$(echo $output | grep -o -P "(?<=Measured Bandwidth with WA \(RFO\) =).*(?=MB/s)")
        printf "%10s %6s %12s\n" $size $ld $bw >> $out_file
    done

    output=$($ld_st_dir/LDST-bench C $compiler $nthread 1 0 $size -c S0:0-$((nthread-1)))
    bw=$(echo $output | grep -o -P "(?<=Measured Bandwidth with WA \(RFO\) =).*(?=MB/s)")
    printf "%10s %6s %12s\n" $size 100 $bw >> $out_file

    printf "\nMachine Info\n" >> $out_file
    lscpu | grep "Model name:" >> $out_file
    lscpu | grep "CPU MHz:" >> $out_file
fi
