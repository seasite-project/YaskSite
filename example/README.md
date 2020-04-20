# README #

This is an example folder for yaskSite

### How do I set up? ###

* mkdir build && cd build (make separate example build directory)
* CC=icc CXX=icpc cmake .. -DyaskSite_DIR=[where yaskSite is installed; prefix provided while installing YASK]
* make perf

### Running yaskSite ###
* ./perf -h (to get help and options)
* Some example stencils are defined in YASK folder, let's try 3D stencil Heat3D:
```
  taskset -c 0-$((threads-1)) ./perf -k Heat3D:3 -m <machine file> -c $threads -t 1 -R 1000:1000:1001 -f 1:1:4 -r 1 -i 10 -O plain
```
* Some example machine files can be found in [mc_files](https://github.com/seasite-project/YaskSite/tree/master/example/mc_files) folder.
