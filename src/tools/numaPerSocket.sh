#!/bin/bash

LANG=en_US.UTF-8

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/"

#USAGE: ./numaPerSocket.sh mc_file
mc_file=$1

numas=$($tool_dir/yamlParser/yamlParser $mc_file "NUMA domains per socket" \
    | cut -d":" -f2)

echo $numas
