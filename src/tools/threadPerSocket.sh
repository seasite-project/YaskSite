#!/bin/bash

LANG=en_US.UTF-8

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/"

#USAGE: ./threadPerSocket.sh mc_file
mc_file=$1

threads=$($tool_dir/yamlParser/yamlParser $mc_file "cores per NUMA domain" \
    | cut -d":" -f2)

echo $threads
