#!/bin/bash

#returns if the file contained in cached directory is a new one
#USAGE: ./needCacheUpdate.sh "stencil_name" "directory_of_stencils" "cache_stencil_dir" "tool_dir" "outputFile"

stencilName=$1
stencilDir=$2
cachedDir=$3
toolDir=$4
outFile=$5
cachedStencil="$cachedDir/stencilCode.hpp"

file=$($toolDir/codeGenerator/getStencilFile.sh $stencilName $stencilDir)

if [[ -e $file ]]
then
    diff -w -q $file $cachedStencil>/dev/null
    diffFlag=$(echo $?)
else
    diffFlag=2
fi

if [[ $diffFlag == 0 ]]
then
    echo "0"
else
    echo "1"
    cp $file $cachedStencil #copy the new stencil
fi
