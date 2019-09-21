#!/bin/bash

#returns if the file contained in cached directory is a new one
#USAGE: ./needCalibration.sh
LANG="C.UTF-8"


diff=1

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mcFile=$(cat $tool_dir/mc_file.txt)

if [[ -e $mcFile ]]
then
    #check machine configurations
    exist_model=$($tool_dir/yamlParser/yamlParser $mcFile "model name")
    exist_model=$(echo $exist_model) #remove white spaces
    curr_model=$(lscpu | grep "Model name:"| cut -d":" -f2)
    curr_model=$(echo $curr_model) #remove white spaces
    if [[ "$exist_model" != "$curr_model" ]]; then
        echo "Warning : Current machine and configuration file do not match"
        diff=0
    fi

    exist_freq=$($tool_dir/yamlParser/yamlParser $mcFile "clock")
    exist_freq=$($tool_dir/unitconvert.sh "$exist_freq" 1000)
    curr_freq=$(lscpu | grep "CPU MHz:"|cut -d ":" -f 2-2)
    curr_freq="$curr_freq MHz"
    curr_freq=$($tool_dir/unitconvert.sh "$curr_freq" 1000)
    exist_freq=$(echo "scale=2; $exist_freq/(1000*1000*1000)" | bc -l)
    curr_freq=$(echo "scale=2; $curr_freq/(1000*1000*1000)" | bc -l)

    if [[ "$exist_freq" != "$curr_freq" ]]; then
        echo Warning : "Frequency of machine ($curr_freq) and configuration file ($exist_freq) do not match"
        diff=0
    fi

    #currently checks only proc 0
    stable_freq=$($tool_dir/cpuFreqCheck.sh)
    if [[ "$stable_freq" != 1 ]]; then
        echo "Warning : Frequency not fixed and/or stable"
        diff=0
    fi
else
    echo "Warning : Machine file not provided"
    diff=0
fi

echo $diff
