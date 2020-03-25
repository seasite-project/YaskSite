#!/bin/bash

#Get the grids in the defined stencil

file=$1
out_file=$2

#grids=$(grep "Updates the following var(s): .*\|Reads the following var(s): .*" $file | cut -d "'" -f 4)
grids=$(grep "Var: .*(" $file | cut -d":" -f 2 | cut -d"(" -f1)

unique_grids=""

#find unique grids, it will be unique, but just let it pass"
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
    dim=$(grep "Var: $u_grid(.*" $file | cut -d":" -f 2 | cut -d"(" -f2 | cut -d")" -f1 | tr "," "\n" | wc -l)
    if [[ $ctr == 0 ]]; then
        printf "%s %d\n" $u_grid $dim > $out_file
    else
        printf "%s %d\n" $u_grid $dim >> $out_file
    fi
    let ctr=$ctr+1
done
