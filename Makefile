PAPER = main
PDFTEX = pdflatex
PSTEX  = latex

default: pdf

pdf:
	$(PDFTEX) $(PAPER)
	bibtex    $(PAPER)
	$(PDFTEX) $(PAPER)
	$(PDFTEX) $(PAPER)

dvi:
	$(PSTEX) $(PAPER)
	bibtex   $(PAPER)
	$(PSTEX) $(PAPER)
	$(PSTEX) $(PAPER)

ps: dvi
	dvips -tletter -o $(PAPER).ps $(PAPER).dvi

clean:
	/bin/rm -f *.dvi *.log *.aux *.toc *.bbl *.blg 
	/bin/rm -f $(PAPER).ps $(PAPER).pdf

