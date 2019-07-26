#!/bin/bash

#KEEPS ST=1 and scans LD from MIN_LD-MAX_LD
#updates only if the file does not exist or does not comply to machine
#USAGE: ./bench_scan.sh LD_ST_bench_DIR COMPILER MIN_LD MAX_LD OUT_FILE

ld_st_dir=$1
compiler=$2
#tool_dir=$3
min_ld=$3
max_ld=$4
#mc_file=$5
out_file=$5

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

mc_file=$(cat $tool_dir/mc_file.txt)
nthread=$($tool_dir/threadPerSocket.sh $mc_file)
numCache=$($tool_dir/cacheInfo/numCache.sh $mc_file)
#llc_size=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $(($numCache-1)))
size=$($tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;MEM;1;size per core;$(($nthread-1))")
size=$($tool_dir/unitconvert.sh "$size")
size=$(echo "$size*10/(1024*1024)" | bc -l)

stable_freq=$(taskset -c 0-0 $tool_dir/bw_bench/cpuFreqCheck.sh)

if [[ "$stable_freq" == "0" ]]; then
    echo "Warning clock speed is not fixed"
fi

cacheStale=$($tool_dir/bw_bench/needCalibration.sh $out_file)


if [[ $cacheStale != "0" ]]; then
    bw_out=""
    ld_st_ratio=""
    echo "Doing bandwidth measurements this could take 1-2 min."

    printf "%10s %6s %12s\n" "Size(MB)" "LD:ST" "BW(MB/s)" > $out_file

    output=$($ld_st_dir/LDST-bench C $compiler $nthread $min_ld 1 $size -c S0:0-$((nthread-1)))
    bw=$(echo $output | grep -o -P "(?<=Measured Bandwidth with WA \(RFO\) =).*(?=MB/s)")
    bw_gbs=$(echo "$bw/1000.0" | bc -l)
    bw_gbs=$(printf '%.*f\n' 2 $bw_gbs)
    bw_out="$bw_gbs GB/s"
    ld_st_ratio="$min_ld"
    printf "%10s %6s %12s\n" $size $min_ld $bw >> $out_file

    for (( ld=$(($min_ld+1)); ld<=$max_ld; ld++ )); do
        #output=$($tool_dir/bw_bench/bench.sh "$tool_dir" "$temp_dir" -1 $ld 1 $size)
        #Using C version, since we need only full socket bandwidths
        output=$($ld_st_dir/LDST-bench C $compiler $nthread $ld 1 $size -c S0:0-$((nthread-1)))
        bw=$(echo $output | grep -o -P "(?<=Measured Bandwidth with WA \(RFO\) =).*(?=MB/s)")
        bw_gbs=$(echo "$bw/1000.0" | bc -l)
        bw_gbs=$(printf '%.*f\n' 2 $bw_gbs)
        bw_out="$bw_out, $bw_gbs GB/s"
        ld_st_ratio="$ld_st_ratio, $ld"
        printf "%10s %6s %12s\n" $size $ld $bw >> $out_file
    done

    output=$($ld_st_dir/LDST-bench C $compiler $nthread 1 0 $size -c S0:0-$((nthread-1)))
    bw=$(echo $output | grep -o -P "(?<=Measured Bandwidth with WA \(RFO\) =).*(?=MB/s)")
    bw_gbs=$(echo "$bw/1000.0" | bc -l)
    bw_gbs=$(printf '%.*f\n' 2 $bw_gbs)
    bw_out="$bw_out, $bw_gbs GB/s"
    ld_st_ratio="$ld_st_ratio, inf"
    printf "%10s %6s %12s\n" $size 100 $bw >> $out_file

    $tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;MEM;1;LDST;ld:st" -o "[$ld_st_ratio]"
    $tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;MEM;1;LDST;BW" -o "[$bw_out]"


    printf "\nMachine Info\n" >> $out_file
    lscpu | grep "Model name:" >> $out_file
    lscpu | grep "CPU MHz:" >> $out_file
fi
