#!/bin/bash

#returns a string of object of specified type
#USAGE ./getObj.sh which_type code_file

#echo string in between two string in a file
type_name="$1"
code_file="$2"
str_2=";"

objs=$(grep -o -P "(?<=$type_name ).*(?=$str_2)" $code_file)
echo $objs | sed 's@,@ @g'
