#!/bin/bash

tool_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
str=$1
echo "$str" | bc $tool_dir/max.bc -l
