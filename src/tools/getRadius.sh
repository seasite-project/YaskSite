#!/bin/bash

findMax()
{
    local x=$1[@]
    max=0
    for val in "${!x}"; do
        if [ $val -gt $max ]
        then
            max=$val
        fi
    done
    echo $max
}

file=$1

halo_x=$(grep ".*halo_x.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')
halo_x=($halo_x)
max_halo_x=$(findMax halo_x)

halo_y=$(grep ".*halo_y.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')
halo_y=($halo_y)
max_halo_y=$(findMax halo_y)

halo_z=$(grep ".*halo_z.*=" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+')
halo_z=($halo_z)
max_halo_z=$(findMax halo_z)

max_halo=($max_halo_x $max_halo_y $max_halo_z)

radius=$(findMax max_halo)

echo $radius
