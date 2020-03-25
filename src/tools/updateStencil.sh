#!/bin/bash

#This script is run before each build to copy SeasiteStencil files and do
#necessary steps to include the new stencils

#SYNTAX ./updateStencil.sh [location of stencils] [location of YASK] [temp_folder location]
locTemplate=$1
stencil_dir=$2

mkdir -p $stencil_dir
cp $locTemplate/*.hpp $stencil_dir/.
stencilFiles=$(find $stencil_dir -type f -path "*.hpp" -printf "%f\n")


for file in $stencilFiles; do
    #replace YASKSITE register with YASK register
    #sed -i "s@YS_REGISTER_STENCIL.*,\(.*\))@static \1 \1_instance@g" $stencil_dir/$file

    classes=$(grep "YS_REGISTER_STENCIL.*" $stencil_dir/$file | cut -d"," -f 2 | cut -d")" -f1)
    #replace with empty in .hpp
    sed -i "s@YS_REGISTER_STENCIL.*);@@g" $stencil_dir/$file

    base_name=$(basename ${file} ".hpp")
    echo "#include \"$file\"" > "${stencil_dir}/${base_name}.cpp"
    for class in $classes;  do
        class_name=$(echo $class)
        echo "static ${class_name} ${class_name}_instance;" >> "${stencil_dir}/${base_name}.cpp"
    done
done
