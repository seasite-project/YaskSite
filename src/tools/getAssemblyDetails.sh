#!/bin/bash

string=$1
file=$2

grep "$string" $file | grep -m1 -Eo '[0-9]'
