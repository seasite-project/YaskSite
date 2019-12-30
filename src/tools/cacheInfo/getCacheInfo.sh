#!/bin/bash

LANG=en_US.UTF-8
tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
tool_dir="$tool_dir/../"

#Usage: ./getCacheInfo.sh $mc_file $cache_id "which info (optional)"
getWays()
{
    mc_file=$1
    cache_id=$2
    ways=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$cache_id;cache per group;ways")
    echo $ways
}

getSets()
{
    mc_file=$1
    cache_id=$2
    sets=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$cache_id;cache per group;sets")
    echo $sets
}

getClSize()
{
    mc_file=$1
    cache_id=$2
    cl_size=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$cache_id;cache per group;cl_size")
    echo $cl_size
}

getNGroups()
{
    mc_file=$1
    cache_id=$2
    n_groups=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$cache_id;groups")
    echo $n_groups
}


getCoresPerCache()
{
    mc_file=$1
    cache_id=$2
    cores_per_group=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$cache_id;cores per group")
    echo $cores_per_group
}

getCacheName()
{
    mc_file=$1
    cache_id=$2
    name=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy" | grep "level:" | head -n $(($cache_id+1)) | tail -n 1 | cut -d ":" -f 2)
    echo $name
}


getTotSize()
{
    mc_file=$1
    cache_id=$2
    totSize=$($tool_dir/yamlParser/yamlParser $mc_file "memory hierarchy;$cache_id;size per group")
    if [ -z "$totSize" ]; then
        ways=$(getWays "$mc_file" "$cache_id")
        sets=$(getSets "$mc_file" "$cache_id")
        cl_size=$(getClSize "$mc_file" "$cache_id")
        totSize=$(echo "($ways*$sets*$cl_size)/1000.0" | bc -l)
        totSize="${totSize} KB"
    fi

    totSize=$($tool_dir/unitconvert.sh "$totSize")
    echo $totSize
}

mc_file=$1

if [[ $3 == "ways" ]]
then
    echo $(getWays "$mc_file" "$2")
elif [[ $3 == "sets" ]]
then
    echo $(getSets "$mc_file" "$2")
elif [[ $3 == "cl_size" ]]
then
    echo $(getClSize "$mc_file" "$2")
elif [[ $3 == "cores" ]]
then
    echo $(getCoresPerCache "$mc_file" "$2")
elif [[ $3 == "name" ]]
then
    echo $(getCacheName "$mc_file" "$2")
else
    echo $(getTotSize "$mc_file" "$2")
fi
