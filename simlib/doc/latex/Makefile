
JMENO=SIMLIB-doc

ps: $(JMENO).ps
$(JMENO).dvi: $(JMENO).tex
	cslatex $(JMENO).tex
	cslatex $(JMENO).tex
	cslatex $(JMENO).tex

$(JMENO).ps: $(JMENO).dvi
	dvips -Z -o $(JMENO).ps $(JMENO).dvi 

pdf: $(JMENO).ps
	ps2pdf $(JMENO).ps $(JMENO).pdf

clean:
	rm -f *.dvi *.log  *.aux *.toc 
clean-all: clean
	rm -f $(JMENO).ps $(JMENO).pdf

pack: clean
	tar czvf $(JMENO).tar.gz *.tex Makefile *.obj *.eps *.cc *.sh *.txt
