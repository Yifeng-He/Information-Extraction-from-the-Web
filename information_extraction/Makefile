# compiler/linker
CXX=g++
LD=g++

#DEBUG mode
DEBUG=
#DEBUG=-DDEBUG

# header files
HEADERS=$(shell pkg-config --cflags glibmm-2.4 libxml++-2.6 --libs)

# compiler/linker flags
CXXFLAGS=-g $(DEBUG) $(HEADERS) -L/usr/local/lib
LDFLAGS=-g $(DEBUG) $(HEADERS) -L/usr/local/lib

# files removalmakefile for opencv program
RM=/bin/rm -f

#library to use when compiling
LIBS=$(shell pkg-config --cflags glibmm-2.4 libxml++-2.6 --libs) -ltidy

#c source files and object files
SRCS=$(wildcard *.cpp)
PROG_OBJS=$(patsubst %.cpp,%.o,$(SRCS))

#program's executable
PROG=HTMLParser

#top-level rule
all: $(PROG)

$(PROG):$(PROG_OBJS)
	$(LD) $(LDFLAGS) $(PROG_OBJS) $(LIBS) -o $(PROG)

#compile cpp source files into object filesmakefile for opencv programmakefile for opencv program
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<

#clean everythinG
clean:
	$(RM) $(PROG_OBJS) $(PROG)

.PHONY: clean


