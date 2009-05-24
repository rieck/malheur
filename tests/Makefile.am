#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES             = @INCLUDES@ -I$(top_srcdir)/src 
TESTS                = test_ftable test_fvec
noinst_PROGRAMS	     = test_ftable test_fvec

test_ftable_SOURCES  = tftable.c tests.c tests.h
test_ftable_LDADD    = $(top_builddir)/src/libmalheur.a

test_fvec_SOURCES    = tfvec.c tests.c tests.h
test_fvec_LDADD	     = $(top_builddir)/src/libmalheur.a
