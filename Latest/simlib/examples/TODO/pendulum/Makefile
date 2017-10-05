# makefile for SIMLIB examples -- use GNU make

MODELS = kyvadlo3

% : %.cc
	SIMLIB $<

# make all models
all: $(MODELS)

# clean garbage
clean:
	rm -f $(MODELS) *.dat *.ps *.eps *.png

# run all models
run: $(MODELS)
	for i in $(MODELS); do ./$$i; done

# plot all results
plot:
	for i in *.plt; do gnuplot ./$$i; done

