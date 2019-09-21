#!/bin/bash

LANG=en_US.UTF-8

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

mc_file=$1 #(cat $tool_dir/mc_file.txt)

numCache=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy"\
    | grep "level:" | wc -l)

echo $(($numCache-1))
