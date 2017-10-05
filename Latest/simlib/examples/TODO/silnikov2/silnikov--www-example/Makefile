
silnikov: silnikov.cc
	SIMLIB $^

run: silnikov.dat
silnikov.dat: silnikov
	./silnikov >silnikov.dat

plot: silnikov.dat
	gnuplot silnikov.plt

clean:
	rm -f silnikov silnikov.dat
