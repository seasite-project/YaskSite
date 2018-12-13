#!/bin/bash

lscpu="LANG=en_US.UTF-8 lscpu"

#USAGE: ./threadPerSocket.sh
nthread=$(eval $lscpu | grep "socket"| cut -d ":" -f 2-2 )
nsocket=$(eval $lscpu | grep "Socket(s)"| cut -d ":" -f 2-2 )
nnuma=$(eval $lscpu | grep "NUMA node(s)"| cut -d ":" -f 2-2 )
numa_per_socket=$(($nnuma/$nsocket))

if [[ $numa_per_socket > 1 ]]; then
    (>&2 echo "COD ON")
fi
nthread=$(($nthread/$numa_per_socket))


echo $nthread
