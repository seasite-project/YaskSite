#!/bin/bash

Sum()
{
    local x=$1[@]
    sum=0
    for val in "${!x}"; do
        let sum=sum+$val
    done
    echo $sum
}

file=$1

alloc_t=$(grep ".*_alloc_t.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')
alloc_t=($alloc_t)

s=$(Sum alloc_t)

echo $s
