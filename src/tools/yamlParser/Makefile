CXX ?= g++
EXTRA_FLAGS =
PREFIX ?= ./
ARCH ?= default
FLAGS = -O3 -std=c++11 -DNDEBUG
LDFLAGS ?=
LIBS ?=
LIB_INC ?=

INCLUDES =

CXXFLAGS = $(FLAGS) $(EXTRA_FLAGS) -I$(LIB_INC)

all: yamlParser

yamlParser: yamlGetInfo.o
	$(CXX) $(CXXFLAGS) -o yamlParser yamlGetInfo.o $(LDFLAGS) $(LIBS)

yamlGetInfo.o: yamlGetInfo.cpp
	$(CXX) -c $(CXXFLAGS) -o yamlGetInfo.o yamlGetInfo.cpp

clean:
	@$(RM) -rf *.o *.i *.s yamlParser
