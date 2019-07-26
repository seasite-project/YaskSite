#!/bin/bash

#updates only if the file does not exist or does not comply to machine
#USAGE: ./prefetch_dist.sh LIKWID_INC LIKWID_LIB

LANG=en_US.UTF-8
tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

likwid_inc=$1
likwid_lib=$2

mc_file=$(cat $tool_dir/mc_file.txt)
SIZE=$($tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;MEM;1;size per thread;1")
SIZE=$($tool_dir/unitconvert.sh "$SIZE")
SIZE=$(echo "$SIZE*10/(1024*1024)" | bc -l) #convert to KB


echo "Currently prefetch distance benchmark assumes ordinary non-victim caches"

LIKWID_INC="-I$likwid_inc" LIKWID_LIB="$likwid_lib" make -C $tool_dir/prefetcher
err_code=$(echo $?)

if [[ $err_code != "0" ]]
then
    echo "Couldn't compile prefetch_dist benchmark. Make sure you have LIKWID and proper destinations set in path"
    exit
fi

echo "Executing prefetch distance benchmark"

exec_out=$(taskset -c 0-0 $tool_dir/prefetcher/bench_exec $SIZE | grep "Prefetch CL:" | cut -d"=" -f 2)

numCaches=$($tool_dir/cacheInfo/numCache.sh $mc_file)

for (( cache=0; cache<numCaches; cache++)); do
    cacheName=$($tool_dir/cacheInfo/getCacheInfo.sh $mc_file $cache "name")
    pre_dist=$( echo $exec_out |  cut -d" " -f $(($cache+1)) )
    $tool_dir/yamlParser/yamlParser $mc_file "benchmarks;measurements;$cacheName;1;prefetch distance" -o "$pre_dist"
done
