#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES             = @INCLUDES@ -I$(top_srcdir)/src 
TESTS                = test_ftable test_fvec test_farray test_fmath \
                       test_quality
noinst_PROGRAMS	     = test_ftable test_fvec test_farray test_fmath \
                       test_quality

test_ftable_SOURCES  = test_ftable.c tests.c tests.h
test_ftable_LDADD    = $(top_builddir)/src/libmalheur.a

test_fmath_SOURCES   = test_fmath.c tests.c tests.h
test_fmath_LDADD     = $(top_builddir)/src/libmalheur.a

test_fvec_SOURCES    = test_fvec.c tests.c tests.h
test_fvec_LDADD      = $(top_builddir)/src/libmalheur.a

test_farray_SOURCES  = test_farray.c tests.c tests.h
test_farray_LDADD    = $(top_builddir)/src/libmalheur.a

test_quality_SOURCES = test_quality.c tests.c tests.h
test_quality_LDADD	 = $(top_builddir)/src/libmalheur.a

