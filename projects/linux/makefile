# makefile -- gnu make
# compiler flags are listed under CPPFLAGS:
#  -Wall	enables most compiler warnings
#  -ggdb	adds debug info to the executable
#  -O0		don't enable any compiler optimizations
#  -03		enable compiler optimization that emphasizes speed over size

# NOTE:
# "-c $< -o $@" is a suffix replacement rule for building .o's from .c's
# by using automatic variables $<: the name of the prerequisite of the 
# rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)

BUILD ?= debug
PROJECT_ROOT = ../..
SOURCE_PATH  = $(PROJECT_ROOT)/src
TARGET       = $(PROJECT_ROOT)/bin/linux/RayTracer

OBJ = $(patsubst %.cpp,%.o,$(wildcard $(SOURCE_PATH)/*.cpp))

CPP = g++
CPPFLAGS = -Wall -ggdb
INCLUDES = -I $(PROJECT_ROOT)/include
LFLAGS = -rdynamic

.PHONY: all clean debug release test

all: debug

clean:
	rm $(SOURCE_PATH)/*.o $(TARGET)

test:
	echo $(OBJ)


debug: CPPFLAGS += -O0
debug: BUILD = debug
debug: $(TARGET)


release: CPPFLAGS += -O3
release: BUILD = release
release: $(TARGET)


$(TARGET): $(OBJ)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(LFLAGS) $^ -o $@

%.o: %.cpp
	$(CPP) $(INCLUDES) -c $< -o $@
