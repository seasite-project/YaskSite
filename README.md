# README #

YaskSite (YASK+SeASiTe) is a library that's help in generating and running well-optimised stencil kernels.
Currently suppports x86 architectures: Intel and AMD. The performance model support is currently available for
star-shaped stencils and its derivatives.


### Features ###

* Uses YASK as back-end
* Dynamic compilation and loading of stencils's
* Code generation for stencil algebra and convolutions
* Analytical tuners for spatial and temporal blocking. Temporal blocking is experimental.
* Easy stencil Input & Output accessors
* Easy transfer between different stencil data format
* Easy plug and go interface for integrating into other applications
* Optional caching of builds for later usage
* Tuning via performance models
* Optional performance monitoring


### How do I set up? ###

See [install_script.sh](https://github.com/seasite-project/YaskSite/blob/master/install_script.sh) for a sample script to install YaskSite and example.
In general following steps should be followed to setup YaskSite:
* `git clone https://github.com/seasite-project/YaskSite.git && cd YaskSite`
* `mkdir build && cd build` (We prefer out of source build so create a separate build directory)
* `CC=icc CXX=icpc cmake .. -DCMAKE_INSTALL_PREFIX=path/to/install`
* To make changes in configuration use `ccmake .` . Configurtions like TEMP_DIR, CACHE_BUILD and LIKWID PATH might be interesting
* `make`
* `make install`
* `make calibrate` - Calibrate the machine on which code has to be run. As input
  a machine file as generated with [kerncraft tool](https://github.com/RRZE-HPC/kerncraft) has to be provided. The
  calibration process then carries out some extra benchmarks and adds to the machine file.

### External dependencies ###

* Intel [YASK](https://github.com/intel/yask)
* Intel [IACA](https://software.intel.com/en-us/articles/intel-architecture-code-analyzer)
* [LIKWID](https://github.com/RRZE-HPC/likwid) - should be manually installed
* [symee](https://github.com/MistFuror/symee)
* [yaml-cpp](https://github.com/jbeder/yaml-cpp)

### How to try it out? ###

* See [example](https://github.com/seasite-project/YaskSite/tree/master/example) folder.
