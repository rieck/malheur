#
# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#

sysconf_DATA = malheur.cfg
man1_MANS = malheur.man 
EXTRA_DIST = malheur.cfg malheur.pod.in malheur.man malheur.txt malheur.pdf \
             doxygen.cfg modules latex-pre.inc.in latex-post.inc

malheur.man: malheur.pod
	pod2man -c "User Manual" -s 1 -r "malheur $(VERSION)" $< > $@

malheur.txt: malheur.pod
	pod2text -w 70 $< > $@

malheur.pdf: malheur.pod latex-pre.inc latex-post.inc
	pod2latex -prefile $(top_builddir)/doc/latex-pre.inc \
	          -postfile $(top_srcdir)/doc/latex-post.inc \
	          -full -out malheur.tex $<
	pdflatex malheur.tex
	pdflatex malheur.tex	
	pdflatex malheur.tex	
	rm -f *.tex *.log *.aux *.toc

latex-pre.inc: latex-pre.inc.in
	sed -e 's|__VERSION__|$(VERSION)|' \
        	$(top_srcdir)/doc/latex-pre.inc.in \
        	> $(top_builddir)/doc/latex-pre.inc

malheur.pod: malheur.pod.in
	sed -e 's|__SYSCONFDIR__|$(sysconfdir)|' \
        	$(top_srcdir)/doc/malheur.pod.in \
        	> $(top_builddir)/doc/malheur.pod

clean-local:
	rm -f *.tex *.log *.aux *.toc *.tmp
	rm -f latex-pre.inc malheur.pod
	
distclean-local:
	rm -f malheur.pdf malheur.man malheur.txt
		
dist-hook: distclean-local malheur.pdf malheur.man
	