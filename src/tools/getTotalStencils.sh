#!/bin/bash

file=$1

totalStencils=$(grep -o "boundaryHandling" "$file" | wc -l)

echo $totalStencils
