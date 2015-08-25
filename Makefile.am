# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)
# University of Goettingen, Berlin Institute of Technology
# --

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src doc tests .
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = README.md INSTALL COPYING CHANGES

clean-local:
	rm -f *~
	rm -f CHANGES

CHANGES:
	$(srcdir)/git2changes.py
	        
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/malheur
