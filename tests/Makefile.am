#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES             = @INCLUDES@ -I$(top_srcdir)/src 
TESTS                = test_ftable test_fvect test_farray test_fmath
noinst_PROGRAMS	     = test_ftable test_fvect test_farray test_fmath

test_ftable_SOURCES  = tftable.c tests.c tests.h
test_ftable_LDADD    = $(top_builddir)/src/libmalheur.a

test_fmath_SOURCES  = tfmath.c tests.c tests.h
test_fmath_LDADD    = $(top_builddir)/src/libmalheur.a

test_fvect_SOURCES    = tfvect.c tests.c tests.h
test_fvect_LDADD      = $(top_builddir)/src/libmalheur.a

test_farray_SOURCES   = tfarray.c tests.c tests.h
test_farray_LDADD     = $(top_builddir)/src/libmalheur.a
