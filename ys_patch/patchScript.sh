yask_folder=$1
which_patch=$2
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for patch_id in $which_patch; do

    if [[ $patch_id == 1 ]]; then
        #Patch 1: Print read grids information
        #only replace pack because this is final
        file="${yask_folder}/src/compiler/lib/Eqs.cpp"
        cp $file $file.orig
        idx=$(grep -n "Updates the following" $file | head -n 1 | cut -d":" -f 1)
        insert_point=$(awk -v "start_idx=${idx}" -v "end_idx=$((idx+20))" '/\\n/ && NR >= start_idx && NR <= end_idx {print NR}' $file | head -n 1 )

        X=1
        Y=$insert_point
        tail -n "+$X" $file | head -n "$((Y-X+1))" > patchTmp.tmp
        cat "${script_dir}/patchText.txt" >> patchTmp.tmp
        X=$((insert_point+1))
        Y=$(wc -l $file | cut -d" " -f1)
        tail -n "+$X" $file | head -n "$((Y-X+1))" >> patchTmp.tmp

        mv patchTmp.tmp $file
    fi

    if [[ $patch_id == 2 ]]; then
        #Patch 2: Use 1234 Layout instead 2314; 
        #this is not mandatory but shows slightly better performance
        file="${yask_folder}/src/compiler/lib/YaskKernel.cpp"
        cp $file $file.orig
        idx=$(grep -n "step_posn = dn + 1;" $file | head -n 1 | cut -d":" -f 1)
        #comment out next line
        sed -i "$((idx+1))s@\(.*\)@//\1@g" $file
        idx=$(grep -n "Add deferred posns at end" $file | head -n 1 | cut -d":" -f 1)
        sed -i "$((idx+1))s@\(.*\)@//\1@g" $file
        sed -i "$((idx+2))s@\(.*\)@//\1@g" $file
    fi

    if [[ $patch_id == 3 ]]; then
        #Patch 3: Avoid if loop inside calc_loop_of_cluster (main loop)
        file="${yask_folder}/src/kernel/lib/realv.hpp"
        cp $file $file.orig
        idx=$(grep -n "storeTo_masked" $file | head -n 1 | cut -d":" -f 1)
        X=1
        Y=$((idx))
        tail -n "+$X" $file | head -n "$((Y-X+1))" > patchTmp.tmp
        cat "${script_dir}/patchText3.txt" >> patchTmp.tmp
        X=$((idx+1))
        Y=$(wc -l $file | cut -d" " -f1)
        tail -n "+$X" $file | head -n "$((Y-X+1))" >> patchTmp.tmp

        mv patchTmp.tmp $file
    fi

    if [[ $patch_id == 4 ]]; then
        #Patch 4: Avoid unnecessary packed unaligned lods
        file="${yask_folder}/src/compiler/lib/Cpp.cpp"
        cp $file $file.orig
        idx=$(grep -n "assert(elems.size()" $file | head -n 1 | cut -d":" -f 1)
        X=1
        Y=$((idx))
        tail -n "+$X" $file | head -n "$((Y-X+1))" > patchTmp.tmp
        cat "${script_dir}/patchText4.txt" >> patchTmp.tmp
        X=$((idx+1))
        Y=$(wc -l $file | cut -d" " -f1)
        tail -n "+$X" $file | head -n "$((Y-X+1))" >> patchTmp.tmp

        mv patchTmp.tmp $file
    fi

done
