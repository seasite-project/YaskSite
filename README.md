# README #

yaskSite (YASK+SeASiTe) is a library that's help in generating and running well-optimised stencil kernels. 



### Features ###

* Uses YASK as back-end
* Dynamic compilation and loading of stencils's
* Code generation for stencil algebra and convolutions
* Analytical tuners for spatial and temporal blocking
* Easy stencil Input & Output accessors
* Easy transfer between different stencil data format
* Easy plug and go interface for integrating into other applications
* Optional caching of builds for later usage
* Tuning via performance models
* Optional performance monitoring 


### How do I get set up? ###

* git clone the repository && cd yasksite
* We prefer out of source build so create a separate build directory (mkdir build)
* CC=icc CXX=icpc cmake .. -DCMAKE_INSTALL_PREFIX=[where to install]
* cmake . (to make configuration changes, TEMP_DIR, CACHE_BUILD and LIKWID PATH might be interesting)
* make
* make install
* make calibrate - Calibrate the machine on which code has to be run.

### Dependencies ###

* YASK
* Intel IACA
* Intel MLC
* LIKWID

### Contribution guidelines ###


### Who do I talk to? ###

Christie Louis Alappat <christie.alappat@fau.de>
