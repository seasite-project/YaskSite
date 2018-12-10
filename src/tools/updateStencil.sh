#!/bin/bash

#This script is run before each build to copy SeasiteStencil files and do
#necessary steps to include the new stencils

#SYNTAX ./updateStencil.sh [location of stencils] [location of YASK] [temp_folder location]
locTemplate=$1
locYASK=$2
tempFolder=$3

stencilFiles=$(find $locTemplate -type f -path "*.hpp" -printf "%f\n")

#update stencil.hpp to include the file
stencil_hpp=$2/src/foldBuilder/stencils.hpp


#make a exclusive folder for yaskSite && copy to proper location
yaskSiteStencilFolder=$locYASK/src/foldBuilder/stencils/yaskSiteStencils

#delete the contents in the folder
rm -f $yaskSiteStencilFolder/*
mkdir -p $yaskSiteStencilFolder

#remove all yaskSiteStencils defined before; this ensures no deprecated stencils resides
sed -i "/.*\"yaskSiteStencils\/.*/d" $stencil_hpp

temp_dir_gen=$tempFolder/temp_dir_generated
mkdir -p $temp_dir_gen

for file in $stencilFiles; do
  #replace YASKSITE register with YASK register
    sed -e "s@"YS_REGISTER_STENCIL.*,"@"REGISTER_STENCIL\("@g" $1/$file > $temp_dir_gen/$file
    sed -i "s@"REGISTER_STENCIL.[^a-zA-Z^\d]"@"REGISTER_STENCIL\("@g" $temp_dir_gen/$file #remove space if present

    cp -u  $temp_dir_gen/$file $yaskSiteStencilFolder/.

    if grep -q ".*\"yaskSiteStencils\/$file\".*" $stencil_hpp; then
        echo "It seems yaskSite stencils from previous build resides"
        sed -i -e "s@.*\"$file\".*@#include \"yaskSiteStencils\/$file\"@g" $stencil_hpp
    else
        echo "#include \"yaskSiteStencils/$file\"">> $stencil_hpp
    fi

done

rm -r $temp_dir_gen
