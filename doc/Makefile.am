#
# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#

BUILT_SOURCES = malheur.txt malheur.man malheur.pdf

sysconf_DATA = malheur.cfg
man1_MANS = malheur.man 
EXTRA_DIST = malheur.cfg malheur.pod malheur.man malheur.pdf malheur.txt \
             doxygen.cfg modules latex-pre.inc latex-post.inc

malheur.man: malheur.pod
	pod2man -c "User Manual" -s 1 -r "malheur $(VERSION)" $< > $@

malheur.txt: malheur.pod
	pod2text -w 72 $< > $@

malheur.pdf: malheur.pod
	pod2latex -prefile latex-pre.inc -postfile latex-post.inc \
	          -full -out malheur.tex $<
	pdflatex malheur.tex
	pdflatex malheur.tex	
	pdflatex malheur.tex	
	rm -f *.tex *.log *.aux *.toc
        
clean-local:
	rm -f malheur.man malheur.pdf malheur.txt
	rm -f *.tex *.log *.aux *.toc
	
        