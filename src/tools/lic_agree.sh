#!/bin/bash

#USAGE: ./lic_agree.sh tool_name readme_file agree_file.txt

tool_name="$1"
readme_file="$2"
agree_file=$3

if [[ ! -e $agree_file ]]; then

    RED='\033[0;31m'
    NC='\033[0m'

    printf "Do you accept the LICENSE AGREEMENT specified by $tool_name toolkit, and agree to its terms and conditions\n"
    printf "Details could be found in $readme_file\n"
    printf "Type ${RED}Y${NC} if agree else ${RED}N${NC}\n"
    read agree

    if [[ $agree != "Y" && $agree != "y" ]]; then
        echo "$tool_name will be switched off, due to non-agreement of license"
        echo "0" >$agree_file
    else
        echo "$tool_name will be used"
        echo "1" >$agree_file
    fi

fi
