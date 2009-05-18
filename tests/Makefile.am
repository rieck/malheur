#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES 	     = @INCLUDES@ -I$(top_srcdir)/src 
TESTS    	     = ftable fvec
#check_PROGRAMS       = ftable fvec
noinst_PROGRAMS	     = ftable fvec

ftable_SOURCES       = ftable.c tests.h
ftable_LDADD	     = $(top_builddir)/src/libmalheur.a

fvec_SOURCES	     = fvec.c tests.h
fvec_LDADD	     = $(top_builddir)/src/libmalheur.a
