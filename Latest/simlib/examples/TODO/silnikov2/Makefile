PROGS = silnikov2

all: $(PROGS)

silnikov2: silnikov2.cc
	SIMLIB $^

run: silnikov2.dat
silnikov2.dat: silnikov2
	./silnikov2 >silnikov2.dat

plot: silnikov2.dat
	gnuplot silnikov2.plt

clean:
	rm -f $(PROGS)

pack: clean
	tar czvf silnikov.tar.gz *.cc *.sh Makefile *.plt

