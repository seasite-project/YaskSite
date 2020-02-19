#!/bin/bash

#USAGE: ./bench.sh TOOL_DIR TEMP_DIR NTHREAD NUM_LOAD NUM_STORE SIZE_PER_THREAD(MB) REPEAT(optional)
tool_dir=$1
temp_dir=$2
nthread=$3
nload=$4
nstore=$5
size=$6
repeat=$7

save_dir=$PWD
#cd $tool_dir/bw_bench

mkdir -p $temp_dir

cp -r $tool_dir/bw_bench $temp_dir/.

cd $temp_dir/bw_bench

#echo string in between two string in a file
cp bench.cpp.in bench.cpp

ld_temp=$(awk '/#UNROLL_BEGIN\(LOAD\)/{flag=1;next}/#UNROLL_END#/{flag=0}flag' bench.cpp | head -n 1)
st_temp=$(awk '/#UNROLL_BEGIN\(STORE\)/{flag=1;next}/#UNROLL_END#/{flag=0}flag' bench.cpp | head -n 1)
var_temp=$(awk '/#UNROLL_BEGIN\(VAR\)/{flag=1;next}/#UNROLL_END#/{flag=0}flag' bench.cpp | head -n 1)
var_dummy_temp=$(awk '/#UNROLL_BEGIN\(VAR_DUMMY\)/{flag=1;next}/#UNROLL_END#/{flag=0}flag' bench.cpp | head -n 1)

echo $ld_temp>ld_template_file.txt
echo $st_temp>st_template_file.txt
echo $var_temp>var_template_file.txt
echo $var_dummy_temp>var_dummy_template_file.txt

#NOW unroll ld and st
ld_unrolled=""
for (( i=0; i<$(($nload-$nstore)); i++ )); do
    ld=$(./substitute.sh "@" "$i" ld_template_file.txt)
    ld_unrolled=$(echo "$ld_unrolled" "$ld")
done
#echo $ld_unrolled

#NOW replace LD with unrolled version
sed -i "/#UNROLL_BEGIN(LOAD)/,/#UNROLL_END#/c\ $ld_unrolled" bench.cpp

st_unrolled=""
for (( i=0; i<$nstore; i++ )); do
    st=$(./substitute.sh "@" "$i" st_template_file.txt)
    st_unrolled=$(echo "$st_unrolled" "$st")
done
#echo $st_unrolled

#NOW replace ST with unrolled version
sed -i "/#UNROLL_BEGIN(STORE)/,/#UNROLL_END#/c\ $st_unrolled" bench.cpp

#NOW unroll var
var_unrolled=""
for (( i=0; i<$(($nload+$nstore)); i++ )); do
    var=$(./substitute.sh "@" "$i" var_template_file.txt)
    var_unrolled=$(echo "$var_unrolled" "$var")
done
#echo $var_unrolled

#NOW replace VAR with unrolled version
sed -i "/#UNROLL_BEGIN(VAR)/,/#UNROLL_END#/c\ $var_unrolled" bench.cpp

#NOW unroll dummy of var
var_dummy_unrolled=""
for (( i=0; i<$(($nload+$nstore)); i++ )); do
    var=$(./substitute.sh "@" "$i" var_dummy_template_file.txt)
    var_dummy_unrolled=$(echo "$var_dummy_unrolled" "$var")
done
#echo $var_dummy_unrolled

#NOW replace VAR with unrolled version
sed -i "/#UNROLL_BEGIN(VAR_DUMMY)/,/#UNROLL_END#/c\ $var_dummy_unrolled" bench.cpp


#generate assembly for reference
#CXX=icpc LOAD=$nload STORE=$nstore SIZE=$size make asm

#remove all vaddpd; to benchmark load and store
#sed -i "s/vaddpd.*//g" bench.s

#now build the benchmark
#CXX=icpc LOAD=$nload STORE=$nstore SIZE=$size make from_asm
CXX=icpc LOAD=$nload STORE=$nstore SIZE=$size make

if [[ $nthread == -1 ]]; then
    nthread=$($tool_dir/threadPerNUMA.sh)
fi
OMP_NUM_THREADS=$nthread KMP_AFFINITY=granularity=fine,compact,1,0 taskset -c 0-$((nthread-1)) ./bench_exec $repeat

cd $save_dir
