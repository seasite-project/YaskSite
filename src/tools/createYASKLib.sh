#!/bin/bash

#This script is a helper to create a shared library for each variant of YASK on
#the fly (during runtime), the created static library will be dynamically linked

#Syntax ./createYASKLib.sh [YASK_FOLDER] [stencil] [arch] [SRC_FORK] [INC_FORK] [LOC_FOLDER]

#might need to update if YASK is updated
YASK_FOLDER=$1
stencil=$2
arch=$3
src_fork=$4
inc_fork=$5
LOC_FOLDER=$6

cc=mpiicpc

#src_files=$(cat "$YASK_FOLDER/Makefile" | grep -E "STENCIL_BASES.*:=.*" | sed -n -e 's/^.*:=//p' | awk '{$0=tolower($0);$1=$1}1')
mv "$YASK_FOLDER/cxx-flags.$stencil.$arch.txt" "$LOC_FOLDER/."
mv "$YASK_FOLDER/ld-flags.$stencil.$arch.txt" "$LOC_FOLDER/."
mv "$YASK_FOLDER/make-report.$stencil.$arch.txt" "$LOC_FOLDER/."
cxx_flags=$(cat "$LOC_FOLDER/cxx-flags.$stencil.$arch.txt")

#copy the current src files from YASK, which includes
#generated files also
cp -r $YASK_FOLDER/src $LOC_FOLDER/.

#copy yaskFork.cpp
cp -r $src_fork/yaskFork.cpp $LOC_FOLDER/src/.
cp -r $inc_fork/yaskFork.h $LOC_FOLDER/src/.

#generate real code
sed -i 's/#ifdef GENERATED/#ifndef GENERATED/g' $LOC_FOLDER/src/yaskFork.cpp

cd $LOC_FOLDER/src
#compile yaskFork
$cc $cxx_flags -I $LOC_FOLDER/src -I $inc_fork -c $LOC_FOLDER/src/yaskFork.cpp
cd -

#make shared library
mkdir -p "$LOC_FOLDER/lib"
echo "creating library..."
echo "$cc -shared -o $LOC_FOLDER/lib/libYASK.so $LOC_FOLDER/src/*.o"
$cc -shared -o $LOC_FOLDER/lib/libYASK.so $LOC_FOLDER/src/*.o

#get assembly code
$YASK_FOLDER/bin/get-loop-stats.pl -p -t=sub_block_loops $YASK_FOLDER/realv_grids.s $YASK_FOLDER/stencil_calc.s $YASK_FOLDER/stencil_main.s $YASK_FOLDER/utils.s > $LOC_FOLDER/assembly.s

#remove .s files from YASK_FOLDER
#remove unwanted files costing disk space
rm -f $YASK_FOLDER/src/*.o
rm -f $YASK_FOLDER/src/*.optrpt
rm -f $YASK_FOLDER/*.s
