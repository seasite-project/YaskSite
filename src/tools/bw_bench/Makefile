CXX?=icpc
LOAD?=2
STORE?=1
SIZE?=100

DEFINES=-DLOAD=$(LOAD) -DSTORE=$(STORE) -DSIZE=$(SIZE)

ifeq ($(CXX), icpc)
	CFLAGS=$(DEFINES) -std=c++11 -qopenmp -O3 -xHOST -fno-alias
else
	CFLAGS=$(DEFINES) -std=c++11 -fopenmp -O3 -Wno-write-strings -ftree-vectorize -march=native
endif

all:
		$(CXX) $(CFLAGS) bench.cpp dummy.cpp -o bench_exec

asm:
		$(CXX) $(CFLAGS) -S bench.cpp

from_asm:
		$(CXX) $(CFLAGS) bench.s dummy.cpp -o bench_exec


clean: 
	rm *.s bench_exec
