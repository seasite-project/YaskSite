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

* git clone the repository
* CC=icc CXX=icpc cmake ..
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
