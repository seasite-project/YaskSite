#!/bin/bash

#Usage : ./memLatency.sh $WRITE_loop[0/1] $numSTREAMS $data_set_size(in bytes) $safety_factor $nthreads $likwid_pin $repeat(optional)

if [[ $1 == "-h" ]]; then
    echo "Usage : ./memLatency.sh WRITE_loop[0/1] numSTREAMS data_set_size(in bytes) safety_factor nthreads likwid_pin repeat(optional)"
    exit
fi

write=$1
streams=$2
mem=$3
sf=$4
nthreads=$5
pin=$6
repeat=$7

if [[ $repeat == "" ]]; then
    repeat=-1
fi

make clean
make WRITE_LOOP=$write STREAMS=$streams

OMP_NUM_THREADS=$nthreads likwid-pin -c $pin ./bench_exec $mem $sf $streams $repeat
