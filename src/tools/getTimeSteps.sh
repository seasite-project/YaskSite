#!/bin/bash

name=$1
file=$2

alloc_t=$(grep "const idx_t ${name}""_alloc_t.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')
if [ -z "$alloc_t" ]; then
    alloc_t=1
fi
echo $alloc_t
