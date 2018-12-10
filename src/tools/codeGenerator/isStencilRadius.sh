#!/bin/bash

#returns id stencil has radius defined
#USAGE: ./isStencilRadius.sh "stencil_name" "directory of stencils" "TOOL_DIR"

stencil_name=$1
directory=$2
TOOL_DIR="$3/codeGenerator"
stencil_file=$($TOOL_DIR/getStencilFile.sh $stencil_name $directory)

if grep -q  "YS_REGISTER_STENCIL_RADIUS.*\(.*\"$stencil_name\".*,.*\)" $stencil_file; then
    echo "1"
else
    echo "0"
fi

