#!/bin/bash

name=$1
file=$2

halo_x=$(grep "$name""_halo_x.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')

halo_y=$(grep "$name""_halo_y.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')

halo_z=$(grep "$name""_halo_z.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')

echo $halo_x
echo $halo_y
echo $halo_z
