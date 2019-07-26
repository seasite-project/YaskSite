#!/bin/bash

#updates only if the file does not exist or does not comply to machine
#USAGE: ./mc_file.sh [out file]

LANG=en_US.UTF-8

read -e -p "Please provide path to machine file:" mc_file
mc_file=$(realpath $mc_file)
printf "Machine file = %s\n" $mc_file
echo $mc_file > $1
