#!/bin/bash

#lscpu="LANG=en_US.UTF-8 lscpu"
#eval $lscpu | grep "CPU MHz:" | cut -d ":" -f 2 | grep -m1 -Eo '[0-9]*\.?[0-9]+'

LANG=en_US.UTF-8
tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
mc_file=$1

freq=$($tool_dir/yamlParser/yamlParser $mc_file "clock")
freq=$($tool_dir/unitconvert.sh "$freq" 1000)
echo "scale=4; $freq/(1000*1000*1000)" | bc -l
