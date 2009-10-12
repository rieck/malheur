#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src doc matlab tests
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = README INSTALL COPYING

mostlyclean-local:
	rm -f *~

install-data-local:
	$(MKDIR_P) $(localstatedir)/malheur
	        