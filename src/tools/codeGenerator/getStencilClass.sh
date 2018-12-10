#!/bin/bash

#returns the class in which the stencil is defined
#USAGE: ./getStencilClass.sh "stencil_name" "stencil_file_name"

stencil_name=$1
stencil_file=$2

class=$(sed -nr "s/YS_REGISTER_STENCIL.*\(.*\"$stencil_name\".*,(.*)\);/\1/p" $stencil_file)

arr=($class)
num_files=${#arr[@]}

if [[ $num_files == 0 ]]; then
    echo "Stencil - $stencil_name not defined"
elif [[ $num_files -ne 1 ]]; then
    echo "ERROR; more than 1 definition for stencil - $stencil_name"
fi

echo $class
