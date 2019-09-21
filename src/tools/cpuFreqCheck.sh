#!/bin/bash

#Program to check whether CPU freq varies
# 0 if not stablle
# 1 if stable
#USAGE: ./cpuFreqCheck.sh

a=200000000000
b=4

sleep 1
freq=$(cat /proc/cpuinfo | grep "cpu MHz" | cut -d ":" -f 2-2 | head -n 1)

for (( i=0; i<=200; i++ )); do
    a=$(bc <<< "scale =3; $a+$a/$b")
    freq="$freq$(cat /proc/cpuinfo | grep "cpu MHz" | cut -d ":" -f 2-2 | head -n 1)"
done

ref_freq=$(echo $freq|cut -d " " -f 1-1)
ctr=1
flag=1
#check if all frequencies are equal
for f in $freq; do
 curr_freq=$(echo $freq|cut -d " " -f $ctr-$ctr)
 if [[ "$curr_freq" != "$ref_freq" ]]; then
     flag=0
     echo $flag
     exit
 fi
 let ctr=$ctr+1
done

echo $flag
