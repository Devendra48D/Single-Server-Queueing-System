# Makefile for SIMLIB (unix)

# TODO: fuzzy extension

.PHONY: doc all clean clean-all test clean-doc pack

all:
	make -C src
	make -C examples
	make -C tests

rebuild-all: clean-all
	make all

32:
	make -C src        32
	make -C examples   32
	make -C tests      32
64:
	make -C src        64
	make -C examples   64
	make -C tests      64

test:
	make -C src        test
test32:
	make -C src        test32

###untested
#fuzzy:
#	make -C src fuzzy

install:
	make -C src install
uninstall:
	make -C src uninstall
# cleaning etc.
clean:
	make -C src clean
	make -C examples clean
	make -C tests clean

### quick test
#test:
#	make -C src test

# doxygen-generated documentation
doc:
	doxygen doxygen.config

clean-doc:
	rm -rf ./doc/doxygen
	make -C ./doc/latex clean

clean-all: clean clean-doc
	make -C examples clean-all
	make -C ./doc/latex clean-all
	make -C tests clean-all

# create archive with .git
pack: clean-all
	(N=`basename $$(pwd)`; cd ..; \
         tar czvf $$N-git-`date +"%Y%m%d"`.tar.gz $$N ; \
         echo "Done.")

# create distribution archive
dist: clean-all
	git log >git-log.txt
	touch . # the main directory should have current time stamp
	(N=`basename $$(pwd)`; cd ..; \
         tar czvf $$N-`date +"%Y%m%d"`.tar.gz --exclude-vcs $$N; \
         echo "Done.")


