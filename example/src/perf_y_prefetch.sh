#!/bin/bash

out_file=$1
kernel=heat3d:3
printf "%5s, %12s, %12s, %12s, %12s, %12s, %12s, %12s, %12s\n" "radius" "L2L1_pred" "L2L1_meas" "L3L2_pred" "L3L2_meas" "MEML3_pred" "MEML3_meas" "MLUPS_pred" "MLUPS_meas" > $out_file

#ensure blocking is L3,L2
for (( radius=1; radius<11; radius++ )); do
    taskset -c 0-3 ./perf_prefetch -k $kernel -c 4 -i 10 -R 1024:100000:10000 -r $radius -f 1:1:4 -i 10 -O spatial > tmp.txt
    L1_pred=$(./getBtwString.sh "L1_pred =" "," tmp.txt)
    L1_measured=$(./getBtwString.sh "L1_measure =" "" tmp.txt)
    L2_pred=$(./getBtwString.sh "L2_pred =" "," tmp.txt)
    L2_measured=$(./getBtwString.sh "L2_measure =" "" tmp.txt)
    L3_pred=$(./getBtwString.sh "L3_pred =" "," tmp.txt)
    L3_measured=$(./getBtwString.sh "L3_measure =" "" tmp.txt)
    mlups_pred=$(./getBtwString.sh "MLUPS_pred =" "," tmp.txt)
    mlups_measured=$(./getBtwString.sh "MLUPS_measure =" "" tmp.txt)

    printf "%5s, %12s, %12s, %12s, %12s, %12s, %12s, %12s, %12s\n" $radius $L1_pred $L1_measured $L2_pred $L2_measured $L3_pred $L3_measured $mlups_pred $mlups_measured >> $out_file
done
