#!/bin/bash

#Get details of equation groups defined in stencil
#O/P Format
#eq_group_name | read_grid_1, read_grid_2, ... | update_grid_1,update_grid_2,... | num_reads | num_writes | num_stencils

file=$1
out_file=$2

rm -f $out_file

eqGroups=$(grep "Equation group '.*with dim =" $file | cut -d "'" -f 2)

unique_eqGroups=""

#find unique grids"
for eqG in $eqGroups; do
    match=0
    for u_eqG in $unique_eqGroups; do
        if [[ $eqG == $u_eqG ]]; then
            let match=1
        fi
    done
    if [[ $match == 0 ]]; then
        unique_eqGroups="$unique_eqGroups $eqG"
    fi
done


#Now find read and write grids of unique_eqGroups
for u_eqG in $unique_eqGroups; do
    read_grid=$(grep "Equation group '$u_eqG' reads grid .*with dim =" $file | cut -d "'" -f 4)
    write_grid=$(grep "Equation group '$u_eqG' updates grid .*with dim =" $file | cut -d "'" -f 4)
    printf "%s " $u_eqG >> $out_file
    read_ctr=0
    for r_g in $read_grid; do
        if [[ $read_ctr == 0 ]]; then
            printf "%s" $r_g >> $out_file
        else
            printf ",%s" $r_g >> $out_file
        fi
        let read_ctr=$read_ctr+1
    done
    printf " " >> $out_file
    write_ctr=0
    for w_g in $write_grid; do
        if [[ $write_ctr == 0 ]]; then
            printf "%s" $w_g >> $out_file
        else
            printf ",%s" $w_g >> $out_file
         fi
        let write_ctr=$write_ctr+1
    done
    num_stencils=$(sed -n -e "/class EqGroup_$u_eqG/,/EqGroup_$u_eqG/p" $file | grep -c \
        "EQUALS.*x+1\|EQUALS.*y+1\|EQUALS.*z+1")
    printf " %d %d %d\n" $read_ctr $write_ctr $num_stencils >> $out_file
done
