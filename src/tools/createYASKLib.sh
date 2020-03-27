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
make_report="yask_kernel.$stencil.$arch.make-report.txt"
#mv "$YASK_FOLDER/build/$make_report" "$LOC_FOLDER/."
cxx_flags=$(cat "$LOC_FOLDER/build/$make_report" | grep -P "^YK_CXXFLAGS=" | cut -d"=" -f2- | sed -e "s@\"@@g")

#copy it to a much convenient location for later parsing
cp $LOC_FOLDER/build/kernel/yask_kernel.$stencil.$arch/gen/yask_stencil_code.hpp $LOC_FOLDER/build/stencil_code.hpp

#include files already in YK_CXXFLAGS
#include="-I${YASK_FOLDER}/include -I${YASK_FOLDER}/src/kernel/lib -I${YASK_FOLDER}/src/common -I${YASK_FOLDER}/src/contrib/coefficients -I${LOC_FOLDER}/build/kernel/yask_kernel.$stencil.$arch/gen"

#copy yaskFork.cpp
cp -r $src_fork/yaskFork.cpp $LOC_FOLDER/build/.
cp -r $inc_fork/yaskFork.h $LOC_FOLDER/build/.
files=$(find  $LOC_FOLDER/build/kernel -type f -name "*.o")
#copy all files in a strech
mv $files $LOC_FOLDER/build/.

#generate real code
sed -i 's/#ifdef GENERATED/#ifndef GENERATED/g' $LOC_FOLDER/build/yaskFork.cpp

cd $LOC_FOLDER/build
#compile yaskFork
echo "compiling"
echo "$cc $cxx_flags -I $inc_fork -c $LOC_FOLDER/build/yaskFork.cpp"
$cc $cxx_flags -I $inc_fork  -x c++ -fPIC -c $LOC_FOLDER/build/yaskFork.cpp

#make shared library
mkdir -p "$LOC_FOLDER/lib"
echo "creating library..."
echo "$cc -shared -o $LOC_FOLDER/lib/libYASK.so $LOC_FOLDER/build/*.o"
$cc -shared -o $LOC_FOLDER/lib/libYASK.so $LOC_FOLDER/build/*.o
cd -

#get assembly code
perl ${YASK_FOLDER}/utils/bin/view_asm.pl -p -l -f=calc_loop_of_clusters -t=yask_stencil_code ${LOC_FOLDER}/asm/factory > $LOC_FOLDER/assembly.s
#$YASK_FOLDER/bin/get-loop-stats.pl -p -t=sub_block_loops $YASK_FOLDER/realv_grids.s $YASK_FOLDER/stencil_calc.s $YASK_FOLDER/stencil_main.s $YASK_FOLDER/utils.s > $LOC_FOLDER/assembly.s

#remove .s files from YASK_FOLDER
#remove unwanted files costing disk space
rm -f $LOC_FOLDER/build/*.o
rm -f $LOC_FOLDER/build/*.optrpt
rm -f $LOC_FOLDER/build/*.s
rm -rf $LOC_FOLDER/bin
rm -rf $LOC_FOLDER/asm
rm -rf $LOC_FOLDER/build/compiler
rm -rf $LOC_FOLDER/build/kernel
rm -f $LOC_FOLDER/lib/libyask_compiler.so
rm -f $LOC_FOLDER/lib/libyask_kernel.*
