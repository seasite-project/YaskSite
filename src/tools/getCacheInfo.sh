#!/bin/bash

getInfo()
{
    Lx=$(lscpu | grep "$1" | cut -d ":" -f 2 | awk '{$0=tolower($0);$1=$1}1')
    Lx_ext=$(echo $Lx|sed 's/[0-9]*//g')
    Lx_val=$(echo $Lx|grep -m1 -Eo '[0-9]*\.?[0-9]+')
    multiple=1

    if [[ $Lx_ext = *"k"* || $Lx_ext = *"K"* ]]; then
        let multiple=1024
    elif [[ $Lx_ext = *"m"* || $Lx_ext = *"M"* ]]; then
        let multiple=1024*1024
    else
        let multiple=1024*1024*1024
    fi

    let Lx=$Lx_val*$multiple
    echo $Lx
}

Lx=$(getInfo "$1")

echo "$Lx"
