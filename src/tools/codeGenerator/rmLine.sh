#!/bin/bash

#deletes the line containing the string in file
#USAGE: ./rmLine.sh string file

str="$1"
file=$2

sed -i "/$str/d" $file
