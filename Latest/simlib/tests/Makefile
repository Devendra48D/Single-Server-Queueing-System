#################################################################
# Makefile for SIMLIB (unix)

MAKEFILE = Makefile.`uname -s`-`uname -m`
MAKEFILE32 = Makefile.`uname -s`-i686
MAKEFILE64 = Makefile.`uname -s`-x86_64

#debug: 
#	make -f Makefile.Linux-i686


#################################################################
all: 
	make -j3 -f $(MAKEFILE)

# special versions for 32/64 bit platforms
# maybe you need make clean first...
32:
	make -f $(MAKEFILE32)
64:
	make -f $(MAKEFILE64)

#################################################################
clean:
	make -f $(MAKEFILE) clean
#	make -C benchmark-64bit clean
#	make -C benchmark-calendar clean
clean-all:
	make -f $(MAKEFILE) clean-all
#	make -C benchmark clean-all
run:
	make -f $(MAKEFILE) run
pack:
	make -f $(MAKEFILE) pack

#################################################################
