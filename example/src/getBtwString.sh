#!/bin/bash

#echo string in between two string in a file
str1="$1"
str2="$2"
file="$3"

grep -o -P "(?<=$str1).*(?=$str2)" $file
