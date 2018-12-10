#!/bin/bash

file=$1
sed -e "s@YS_REGISTER_STENCIL@REGISTER_STENCIL@g" $file
