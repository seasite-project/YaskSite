#!/bin/bash

#This script is run before each build to copy SeasiteStencil files and do
#necessary steps to include the new stencils

#SYNTAX ./updateStencil.sh [location of SeasiteStencil.hpp] [location of YASK]
locTemplate=$1
locYASK=$2

#copy to proper location
cp -u $1/SeasiteStencil.hpp $2/src/foldBuilder/stencils/.

#update stencil.hpp to include the file
stencil_hpp=$2/src/foldBuilder/stencils.hpp

if grep -q '.*SeasiteStencil.hpp.*' $stencil_hpp; then
    sed -i -e 's/.*SeasiteStencil.hpp.*/#include "SeasiteStencil.hpp"/g' $stencil_hpp
else
    echo '#include "SeasiteStencil.hpp"'>> $stencil_hpp
fi
