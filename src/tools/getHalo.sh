#!/bin/bash

name=$1
file=$2

function getHalo
{
    dir=$1
    left_halo=$(grep "$name""_left_halo_${dir}.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')
    right_halo=$(grep "$name""_right_halo_${dir}.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')

    if [[ "$left_halo" != "$right_halo" ]]; then
        echo "Halo not homogeneous in $dir direction, currently YaskSite only supports homogeneous radius"
    else
        echo $right_halo
    fi
}

halo_x=$(getHalo "x")
halo_y=$(getHalo "y")
halo_z=$(getHalo "z")

echo $halo_x
echo $halo_y
echo $halo_z
