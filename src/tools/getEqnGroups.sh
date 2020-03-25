#!/bin/bash

#Get details of equation groups defined in stencil
#O/P Format
#eq_group_name | read_grid_1, read_grid_2, ... | update_grid_1,update_grid_2,... | num_reads | num_writes | num_stencils

file=$1
stencil_code_file=$2
out_file=$3

rm -f $out_file

eqGroups=$(grep "equation-bundle '.*" $file | cut -d "'" -f 2 | cut -d"'" -f1)

unique_eqGroups=""

unique_ctr=0
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
        let unique_ctr=$unique_ctr+1
    fi
done
#numEqs=$(grep -oP "#define NUM_STENCIL_EQS \K\w+" $stencil_code_file)
#if [[ "$numEqs" != "$unique_ctr" ]]; then
#    echo "Error: Eq. group not correctly parsed, see $numEqs != $unique_ctr"
#fi

#Now find read and write grids of unique_eqGroups
for u_eqG in $unique_eqGroups; do
    start_idx=$(grep -n "equation-bundle '$u_eqG" $file | cut -d ":" -f 1 | head -n1)
    end_idx=$((start_idx+3))

    write_grids_str=$(tail -n "+$start_idx" $file | head -n "$((end_idx-start_idx+1))" | grep "Updates the following var(s): .*" | cut -d":" -f2)
    write_grids_wo_space=$(echo ${write_grids_str//[[:blank:]]/})
    write_grids_w_comma=$(echo $write_grids_wo_space | rev | cut -c 2- | rev) #remove full stop
    write_grids=$(echo $write_grids_w_comma | sed "s@,@ @g")

    read_grids_str=$(tail -n "+$start_idx" $file | head -n "$((end_idx-start_idx+1))" | grep "Reads the following var(s): .*" | cut -d":" -f2)
    read_grids_wo_space=$(echo ${read_grids_str//[[:blank:]]/})
    read_grids_w_comma=$(echo $read_grids_wo_space | rev | cut -c 2- | rev) #remove full stop
    read_grids=$(echo $read_grids_w_comma | sed "s@,@ @g")

    printf "%s " $u_eqG  >> $out_file

    read_ctr=0 #$(echo $read_grids | tr "," "\n" | wc -l)
    new_read_grids=""
    for r_g in $read_grids; do
        #check if string not empty; because it can be empty
        if [[ ! -z "${r_g// }" ]]; then
            if [[ $read_ctr == 0 ]]; then
                printf "%s" $r_g >> $out_file
            else
                printf ",%s" $r_g >> $out_file
            fi
            new_read_grids="${new_read_grids} ${r_g}"
            let read_ctr=$read_ctr+1
        fi
    done
    printf " " >> $out_file

    write_ctr=0 #$(echo $write_grids | tr "," "\n" | wc -l)
    new_write_grids=""
    for w_g in $write_grids; do
        if [[ ! -z "${w_g// }" ]]; then
            if [[ $write_ctr == 0 ]]; then
                printf "%s" $w_g >> $out_file
            else
                printf ",%s" $w_g >> $out_file
            fi
            let write_ctr=$write_ctr+1
            new_write_grids="${new_write_grids} ${r_g}"
        fi
    done

    num_stencils=0
    #find num stencils
    for ((i=1; i<=$read_ctr; i=$i+1)); do
        #cur_num_stencils=$(sed -n -e "/class StencilBundle_$u_eqG/,/StencilBundle_$u_eqG/p" $stencil_code_file | grep -c \
        #    "EQUALS.*x +\|EQUALS.*y +\|EQUALS.*z +\|EQUALS.*x -\|EQUALS.*y -\|EQUALS.*z -")

        stencil_line=$(sed -n -e "/class StencilBundle_$u_eqG/,/StencilBundle_$u_eqG/p" $stencil_code_file | grep "EQUALS")
        read_grid=$(echo $new_read_grids | cut -d" " -f$i)
        cur_num_stencils_a=$(echo $stencil_line | grep -zPo " ${read_grid}(\(([^()]++|(?1))*\))" | grep -c "x +\|y +\|z +\|x -\|y -\|z -")
        cur_num_stencils_b=$(echo $stencil_line | grep -zPo "\(${read_grid}(\(([^()]++|(?1))*\))" | grep -c "x +\|y +\|z +\|x -\|y -\|z -")
        if [[ "${cur_num_stencils_a}" != "0" ]] || [[ "${cur_num_stencils_b}" != "0" ]]; then
            let num_stencils=$num_stencils+1
        fi
    done
    printf " %d %d %d\n" $read_ctr $write_ctr $num_stencils >> $out_file
done
