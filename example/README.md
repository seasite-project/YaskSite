# README #

This is an example folder for yaskSite

### How do I set up? ###

* `mkdir build && cd build`. Make separate example build directory
* `CC=icc CXX=icpc cmake .. -DyaskSite_DIR=[where YaskSite is installed; CMAKE_INSTALL_PREFIX of YaskSite]`
* `make`

### Running yaskSite ###
* `./perf -h` (to get help and options)
* Some example stencils are defined in YASK folder, let's try 3D stencil Heat3D:
```
  taskset -c 0-$((threads-1)) ./perf -k Heat3D:3 -m <machine file> -c $threads -t 1 -R 1000:1000:1001 -f 1:1:4 -r 1 -i 10 -O plain
```
* Use `perf_wo_likwid` instead of `perf` in case no LIKWID is installed. In this case no validation will be done.
* Some example machine files can be found in [mc_files](https://github.com/seasite-project/YaskSite/tree/master/example/mc_files) folder.
