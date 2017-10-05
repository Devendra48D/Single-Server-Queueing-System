###################################################################
# Makefile for fuzzy extension of SIMLIB
# Author: David Martinek
# Date:   4.4.2001

#TODO: change directory structure 

FUZZY_SRC=.
XERCES_DIR=$(FUZZY_SRC)/xerces
XERCES_LIB=$(XERCES_DIR)/lib/libxerces-c1_4.so
SIMLIB_SRC=../../src
SIMLIB_DIR=$(SIMLIB_SRC)
SIMLIB_DEPEND=$(SIMLIB_SRC)/simlib.h
FUZZY_DEPEND=$(SIMLIB_DEPEND) $(FUZZY_SRC)/fuzzy/fuzzy.h
#XERCES_LIB=-lxerces-c1_4

CXX=g++
CXXFLAGS = -O2 -g -Wall

ILIBS= -I$(SIMLIB_DIR) -I. -I$(SIMLIB_SRC) -I$(SIMLIB_SRC)/fuzzy -I$(XERCES_DIR)/include
LINKLIBS=$(SIMLIB_DIR)/simlib.so  $(XERCES_LIB) -lm
%.o : %.cc
	$(CXX) $(CXXFLAGS) $(ILIBS) -c $<
###################################################################

fuzzyanalyzer.o: fuzzyanalyzer.cc $(FUZZY_DEPEND) analyzer.h fuzzyanalyzer.h strx.h

clean:
	rm -f *.dat *.o
#
