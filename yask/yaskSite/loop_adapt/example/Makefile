

CFLAGS   = -g -O2 -std=gnu99 -Wno-format -fPIC
INCLUDES = -I..
LIBPATH = -L..
LIBS = -lloop_adapt

all: inf_memcpy

inf_memcpy: inf_memcpy.c
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBPATH) inf_memcpy.c -o $@ $(LIBS)

clean:
	rm -f inf_memcpy
