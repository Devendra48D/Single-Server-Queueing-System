###################################################################
# Makefile for fuzzy extension of SIMLIB
# Author: David Martinek
# Date:   4.4.2001

SIMLIB_SRC=../src
SIMLIB_DIR=$(SIMLIB_SRC)
SIMLIB_DEPEND=$(SIMLIB_SRC)/simlib.h
FUZZY_DEPEND=$(SIMLIB_DEPEND) fuzzy.h

#CXX=g++
#CXXFLAGS=-O2 -g -Wall

ILIBS= -I$(SIMLIB_DIR) -I. -I$(SIMLIB_SRC)
LINKLIBS=$(SIMLIB_DIR)/simlib.so  -lm
%.o : %.cc
	$(CXX) $(CXXFLAGS) $(ILIBS) -c $<
###################################################################
OBJS=fuzzy.o fuzzyio.o fuzzymf.o fuzzyrul.o ruletree.o rules.o 

# pøelo¾í v¹echny moduly
all: $(OBJS)

fuzzy.o:    fuzzy.cc    $(FUZZY_DEPEND)
fuzzyio.o:  fuzzyio.cc  $(FUZZY_DEPEND)
fuzzymf.o:  fuzzymf.cc  $(FUZZY_DEPEND)
fuzzyrul.o: fuzzyrul.cc $(FUZZY_DEPEND)
ruletree.o: ruletree.cc $(FUZZY_DEPEND)
rules.o:    rules.cc    $(FUZZY_DEPEND)

clean:
	rm -f *.dat *.o

doc:
	doxygen doxygen.config
#
