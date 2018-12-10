YASK_SITE_INC_DIR=~/installkit/include/yaskSite/
YASK_SITE_LIB_DIR=~/installkit/lib/yaskSite/
YASK_SITE_LIB=yaskSite
LOOP_ADAPT_LIB_DIR=~/yask/yaskSite/build/loop_adapt-inst/lib/
LOOP_ADAPT_LIB=loop_adapt
DEPS =
OBJ = main.o
CC=mpiicpc

CFLAGS=-I . -I $(YASK_SITE_INC_DIR) -std=c++11 -qopenmp -Wl,-rpath $(YASK_SITE_LIB_DIR) -Wl,-rpath $(LOOP_ADAPT_LIB_DIR) -L $(YASK_SITE_LIB_DIR) -L $(LOOP_ADAPT_LIB_DIR) $(LIKWID_LIB) -llikwid 

%.o: %.cpp $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

all: yaskSite perf

yaskSite: $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) -l $(YASK_SITE_LIB) -l $(LOOP_ADAPT_LIB)

perf: perf.o
		$(CC) -o $@ $^ $(CFLAGS) -l $(YASK_SITE_LIB)

clean: 
	rm *.o yaskSite perf
