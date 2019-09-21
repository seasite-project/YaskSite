#!/bin/bash

topology=$(likwid-topology | grep "Socket 0:" | cut -d":" -f 2 | sed "s/[(|)]//g")
topology=$(echo $topology)
skip_dist=$1
offset=$2
totalNum=$(echo $topology | tr " "  "\n" | wc -l)
num=$3

cpu_list=""


for ((idx=$((offset)); idx<totalNum; idx=$idx+skip_dist)); do
    cpu=$(echo $topology | cut -d" " -f$((idx+1)))
    if [[ $cpu_list == "" ]]; then
        cpu_list=$cpu
    else
        cpu_list="$cpu_list"",""$cpu"
    fi
done

selectedCpus=$(echo $cpu_list | cut -d"," -f1-$num)
echo $selectedCpus


