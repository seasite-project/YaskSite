#!/bin/bash

name=$1
file=$2

alloc_t=$(grep "$name""_alloc_t.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')

echo $alloc_t

