#!/bin/bash

LANG=en_US.UTF-8
tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

mc_file=$1
numCache=$($tool_dir/cacheInfo/numCache.sh $mc_file)
size=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$numCache;size per group")
size=$($tool_dir/unitconvert.sh "$size")
echo $size
