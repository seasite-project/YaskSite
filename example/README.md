# README #

This is an example folder for yaskSite

### How do I get set up? ###

* make separate build directory (mkdir build)
* CC=icc CXX=icpc cmake .. -DyaskSite_DIR=[where yaskSite is installed; prefix provided while installing]
* make perf

### Running yaskSite ###
* ./perf -h (to get help and options)
* Example with heat3d
  taskset -c 0-$((threads-1)) ./perf -k heat3d:3 -c $threads -t 1 -R 1400:1000:1500 -f 1:1:4 -r 1 -i 10 -O temporal

### Who do I talk to? ###

Christie Louis Alappat <christie.alappat@fau.de>
