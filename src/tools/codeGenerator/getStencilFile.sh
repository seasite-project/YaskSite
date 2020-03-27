#!/bin/bash

#returns the file in which the stencil is defined
#USAGE: ./getStencilFile.sh "stencil_name" "directory_of_stencils"

stencil_name="$1"
directory="$2"

file=$(grep -Rl "YS_REGISTER_STENCIL.*(.*\"$stencil_name\".*," $directory)

arr=($file)
num_files=${#arr[@]}

if [[ $num_files == 0 ]]; then
   >&2  echo "Stencil - $stencil_name not defined"
elif [[ $num_files -ne 1 ]]; then
   >&2 echo "ERROR; more than 1 definition for stencil - $stencil_name"
fi

echo $file
