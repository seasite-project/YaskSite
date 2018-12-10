#!/bin/bash

echo "tmp = STENCIL(DATA<0>);"
echo "F"_1"<0> = tmp"_0"*h + DATA<0>;"
echo "F[ll]<0> = tmp"_1"*h + DATA<0>;"
echo "F"_14"<0> = tmp"_2"*h + DATA<0>;"
echo "F"_15"<0> = tmp"_3"*h + DATA<0>;"
echo ""
