LIKWID_LIB ?= -L/usr/local/lib
LIKWID_INC ?= -I/usr/local/include
LIKWID_LIBS ?= -llikwid

CXX?=icpc
LOAD?=2
STORE?=1
SIZE?=100

EXTRA_DEFINES ?=
DEFINES=-DLIKWID_PERFMON

BUILD_CFLAGS := $(LIKWID_LIB) $(LIKWID_INC) -I. $(REQUIRED_CFLAGS) $(CFLAGS)
BUILD_LFLAGS := $(LIKWID_LIBS)


ifeq ($(CXX), icpc)
	CFLAGS=$(DEFINES) $(EXTRA_DEFINES) -std=c++11 -qopenmp -O3 -xHOST -fno-alias
else
	CFLAGS=$(DEFINES) $(EXTRA_DEFINES) -std=c++11 -fopenmp -O3 -Wno-write-strings -ftree-vectorize -march=native
endif

all: bench.o dummy.o
		$(CXX) $(CFLAGS) $(BUILD_CFLAGS) bench.o dummy.o -o bench_exec $(BUILD_LFLAGS)
bench.o: bench.cpp
		$(CXX) $(CFLAGS) $(BUILD_CFLAGS) -c bench.cpp -o bench.o
dummy.o: dummy.cpp
		$(CXX) $(CFLAGS) $(BUILD_CFLAGS) -c dummy.cpp -o dummy.o



clean: 
	rm bench_exec
