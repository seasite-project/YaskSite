#!/bin/bash

file=$1

#totalStencils=$(grep -o "boundaryHandling" "$file" | wc -l)
base=$(grep -o "yask_compiler_api.hpp" $file | wc -l) #basic stencil
totalStencils=$(grep -o "addPoints(.*z)" "$file" | wc -l)

if [[ $base == "1" ]]; then
    echo -1
else
    echo $totalStencils
fi
