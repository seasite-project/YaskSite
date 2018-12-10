LIKWID_LIB ?= -L/usr/local/lib
LIKWID_INC ?= -I/usr/local/include
LIKWID_LIBS ?= -llikwid
CC ?= gcc

REQUIRED_CFLAGS := -pthread

CFLAGS := -O3

BUILD_CFLAGS := $(LIKWID_LIB) $(LIKWID_INC) -I. $(REQUIRED_CFLAGS) $(CFLAGS)
BUILD_LFLAGS := $(LIKWID_LIBS)

all: mlc_no_pf

mlc_no_pf: mlc_no_pf.c
	$(CC) $(BUILD_CFLAGS) mlc_no_pf.c -o mlc_no_pf $(BUILD_LFLAGS)

clean:
	rm -f mlc_no_pf

.PHONY: clean
