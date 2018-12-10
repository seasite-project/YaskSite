#!/bin/bash

#Get the grids in the defined stencil

file=$1
out_file=$2

grids=$(grep "reads grid '.*with\|updates grid '.*with" $file | cut -d "'" -f 4)

unique_grids=""

#find unique grids"
for grid in $grids; do
    match=0
    for u_grid in $unique_grids; do
        if [[ $grid == $u_grid ]]; then
            let match=1
        fi
    done
    if [[ $match == 0 ]]; then
        unique_grids="$unique_grids $grid"
    fi
done

ctr=0

#find dimensions of unique grids
for u_grid in $unique_grids; do
    dim=$(grep "reads grid '$u_grid' with dim\|updates grid '$u_grid' with dim" $file | cut -d "=" -f 2 | grep  -Eo '[0-9]*\.?[0-9]+' | head -n 1)
    if [[ $ctr == 0 ]]; then
        printf "%s %d\n" $u_grid $dim > $out_file
    else
        printf "%s %d\n" $u_grid $dim >> $out_file
    fi
    let ctr=$ctr+1
done

