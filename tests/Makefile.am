#
# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009 Konrad Rieck (konrad.rieck@tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES                = @INCLUDES@ -I$(top_srcdir)/src 
TESTS                   = test_ftable test_fvec test_farray test_fmath \
                          test_quality test_proto test_cluster \
                          test_class
noinst_PROGRAMS         = test_ftable test_fvec test_farray test_fmath \
                          test_quality test_proto test_cluster \
                          test_class

test_ftable_SOURCES     = test_ftable.c tests.c tests.h
test_ftable_LDADD       = $(top_builddir)/src/libmalheur.a

test_fmath_SOURCES      = test_fmath.c tests.c tests.h
test_fmath_LDADD        = $(top_builddir)/src/libmalheur.a

test_fvec_SOURCES       = test_fvec.c tests.c tests.h
test_fvec_LDADD         = $(top_builddir)/src/libmalheur.a

test_farray_SOURCES     = test_farray.c tests.c tests.h
test_farray_LDADD       = $(top_builddir)/src/libmalheur.a

test_quality_SOURCES    = test_quality.c tests.c tests.h
test_quality_LDADD      = $(top_builddir)/src/libmalheur.a

test_proto_SOURCES      = test_proto.c tests.c tests.h
test_proto_LDADD        = $(top_builddir)/src/libmalheur.a

test_cluster_SOURCES    = test_cluster.c tests.c tests.h
test_cluster_LDADD      = $(top_builddir)/src/libmalheur.a

test_class_SOURCES   = test_class.c tests.c tests.h
test_class_LDADD     = $(top_builddir)/src/libmalheur.a

beautify:               
			gnuindent -i4 -kr -l77 -lc77 --no-tabs -cs \
                        -T FILE -T fvec_t -T farray_t -T cluster_t \
                        -T assign_t -T config_t -T config_setting_T \
                        -T DIR -T DIRP *.c *.h
                        
