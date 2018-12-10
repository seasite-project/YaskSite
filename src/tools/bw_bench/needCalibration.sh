#!/bin/bash

#returns if the file contained in cached directory is a new one
#USAGE: ./needCalibration.sh "calib_file"

calibFile=$1

diff=0

if [[ -e $calibFile ]]
then
    #check machine configurations
    exist_model=$(cat $calibFile | grep "Model name:" | cut -d ":" -f 2-2)
    curr_model=$(lscpu | grep "Model name:"|cut -d ":" -f 2-2)
    if [[ "$exist_model" != "$curr_model" ]]; then
        echo "Machine calibration required different models"
        diff=1
    fi

    exist_freq=$(cat $calibFile | grep "CPU MHz:" | cut -d ":" -f 2-2)
    curr_freq=$(lscpu | grep "CPU MHz:"|cut -d ":" -f 2-2)
    if [[ "$exist_freq" != "$curr_freq" ]]; then
        echo "Machine calibration required different frequency"
        diff=1
    fi
else
    diff=2
fi

echo $diff
