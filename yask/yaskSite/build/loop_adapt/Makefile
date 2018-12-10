
include config.mk
LIKWID_INCDIR ?= /usr/local/include
LIKWID_LIBDIR ?= /usr/local/lib
INCLUDES  += -I. -I$(LIKWID_INCDIR)
OBJ       = $(patsubst %.c, %.o,$(wildcard *.c))
Q         ?= @
DEFINES += -D_GNU_SOURCE -DLIKWID_PERFMON

ANSI_CFLAGS   =

CFLAGS   = -g -O2 -std=gnu99 -Wno-format -fPIC
SHARED_CFLAGS = -fPIC
SHARED_LFLAGS = -shared -L$(LIKWID_LIBDIR)
DYNAMIC_TARGET_LIB = libloop_adapt.so
STATIC_TARGET_LIB = libloop_adapt.a
LIBS = -llikwid


all: $(DYNAMIC_TARGET_LIB) $(STATIC_TARGET_LIB) examples

$(DYNAMIC_TARGET_LIB): $(OBJ)
	@echo "===>  Link  $@"
	$(Q)${CC} $(DEFINES) $(INCLUDES) $(SHARED_LFLAGS) $(CFLAGS) $(ANSI_CFLAGS) $(SHARED_CFLAGS) -o $(DYNAMIC_TARGET_LIB) $(OBJ) $(LIBS)

$(STATIC_TARGET_LIB): $(OBJ)
	@echo "===>  Link  $@"
	$(Q)ar rcs $(STATIC_TARGET_LIB) $(OBJ)

%.o:  %.c
	@echo "===>  COMPILE  $@"
	$(Q)$(CC) $(DEFINES) $(INCLUDES) -c $(CFLAGS) $(ANSI_CFLAGS) $(CPPFLAGS) $< -o $@

examples:
	make -C example all

install: $(DYNAMIC_TARGET_LIB)
	$(Q)install -m 755 $(DYNAMIC_TARGET_LIB) $(PREFIX)/lib/$(DYNAMIC_TARGET_LIB)
	$(Q)install -m 755 $(STATIC_TARGET_LIB) $(PREFIX)/lib/$(STATIC_TARGET_LIB)
	cp *.h $(PREFIX)/include/.

uninstall:
	$(Q)rm $(PREFIX)/lib/$(DYNAMIC_TARGET_LIB) $(PREFIX)/lib/$(STATIC_TARGET_LIB)
	$(Q)rm $(PREFIX)/include/loop_adapt.h

clean:
	make -C example clean
	@rm -rf *.o $(DYNAMIC_TARGET_LIB) $(STATIC_TARGET_LIB)

.PHONY: clean
