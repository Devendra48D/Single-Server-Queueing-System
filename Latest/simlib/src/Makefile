#################################################################
# Makefile for SIMLIB (unix)

MAKEFILE = Makefile.`uname -s`-`uname -m`
MAKEFILE32 = Makefile.`uname -s`-i686
MAKEFILE64 = Makefile.`uname -s`-x86_64

#current-debuging-target: 32

#################################################################
all: 
	make -j3 -f $(MAKEFILE)

# special versions for 32/64 bit platforms
# maybe you need make clean first...
32:
	make -f $(MAKEFILE32)
64:
	make -f $(MAKEFILE64)

#TODO: change
fuzzy:
	make -f $(MAKEFILE)  MODULES="fuzzy" fuzzymodule all

#################################################################
dep:
	make -f $(MAKEFILE) dep
#################################################################
test:
	make -f $(MAKEFILE) test
test32:
	make -f $(MAKEFILE32) test
#################################################################
install:
	make -f $(MAKEFILE) install
uninstall:
	make -f $(MAKEFILE) uninstall
#################################################################
clean:
	make -f $(MAKEFILE) clean
	make -f $(MAKEFILE) MODULES="fuzzy" clean
pack:
	make -f $(MAKEFILE) pack
#################################################################
