#!/bin/bash

#USAGE: ./threadPerSocket.sh
nthread=$(lscpu | grep "socket"| cut -d ":" -f 2-2 )
nsocket=$(lscpu | grep "Socket(s)"| cut -d ":" -f 2-2 )
nnuma=$(lscpu | grep "NUMA node(s)"| cut -d ":" -f 2-2 )
numa_per_socket=$(($nnuma/$nsocket))

if [[ $numa_per_socket > 1 ]]; then
    (>&2 echo "COD ON")
fi
nthread=$(($nthread/$numa_per_socket))


echo $nthread
