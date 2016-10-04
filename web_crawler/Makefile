# compiler/linker
CXX=g++
LD=g++

#DEBUG mode
DEBUG=
#DEBUG=-DDEBUG

# header files
HEADERS=$(shell pkg-config --cflags glibmm-2.4 libxml++-2.6 --libs) -I/usr/local/include/curlplusplus-1.2/ -I/usr/local/include/tidypp-1.0/ -I/usr/include/mysql

# compiler/linker flags
CXXFLAGS=-g $(DEBUG) $(HEADERS) 
LDFLAGS=-g $(DEBUG) $(HEADERS) 

# remove files
RM=/bin/rm -f -r

#library to use when compiling
LIBS=$(shell pkg-config --cflags glibmm-2.4 libxml++-2.6 --libs) -ltidy -lcurl -lm /usr/local/lib/libcurlplusplus-1.2.la /usr/local/lib/libtidypp-1.0.la -lmysqlclient -lboost_system -lboost_regex  -lboost_date_time -lboost_thread -pthread -lboost_serialization

#c source files and object files
SRCS=$(wildcard *.cpp)
PROG_OBJS=$(patsubst %.cpp,%.o,$(SRCS))

#program's executable
PROG=testWebDataExtraction

#folder to store the data
FOLDER=data

#top-level rule
all: $(PROG)

$(PROG):$(PROG_OBJS)
	libtool --mode=link $(LD) -o $(PROG) $(PROG_OBJS) $(LIBS) -L/usr/local/lib
	mkdir -p $(FOLDER)

#compile cpp source files into object files
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<

#clean everything
clean:
	$(RM) $(PROG_OBJS) $(PROG) $(FOLDER)

.PHONY: clean


