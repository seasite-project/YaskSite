#!/bin/bash

#Feel free to edit
install_dir="~/installkit"
tmp_dir="~/tmp_YaskSite"

#BUILD YaskSite
mkdir build && cd build
CC=icc CXX=icpc cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DTEMP_DIR=${tmp_dir}
make install

#BUILD examples
cd example
mkdir build && cd build
CC=icc CXX=icpC cmake .. -DyaskSite_DIR=${install_dir}
make

#Set PATH to example folder for Offsite.
#Optional only needed if you need to use it with external tool like Offsite
PATH="${PATH}:${PWD}/example"
