# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
# University of Goettingen, Berlin Institute of Technology
# --

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src doc matlab tests .
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = README.md INSTALL COPYING CHANGES

clean-local:
	rm -f *~
	        
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/malheur
