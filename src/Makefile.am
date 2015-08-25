# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)
# University of Goettingen, Berlin Institute of Technology

EXTRA_DIST		= malheur.h.in
BUILT_SOURCES		= malheur.h

noinst_LIBRARIES        = libmalheur.a			  
libmalheur_a_SOURCES	= md5.c md5.h common.h util.c util.h fvec.c \
                          fvec.h ftable.c ftable.h mist.c \
                          mist.h fmath.c fmath.h export.c export.h \
                          farray.c farray.h proto.c proto.h mconfig.c \
                          mconfig.h cluster.c cluster.h quality.c \
                          quality.h class.c class.h uthash.h \
                          murmur.c murmur.h

bin_PROGRAMS            = malheur 
malheur_SOURCES    	= $(libmalheur_a_SOURCES) malheur.c
malheur_DEPENDENCIES	= malheur.h
malheur_LDFLAGS         = -static

malheur.h: malheur.h.in
	cat $(top_srcdir)/src/malheur.h.in \
	| sed -e 's|__SYSCONFDIR__|$(sysconfdir)|' \
	| sed -e 's|__LOCALSTATEDIR__|$(localstatedir)|' \
        > malheur.h
         
clean-local:
	rm -f 		malheur.h

beautify:               $(libmalheur_a_SOURCES) $(malheur_SOURCES)
			gindent -i4 -kr -l77 -lc77 --no-tabs -cs \
                        -T FILE -T fvec_t -T farray_t -T cluster_t \
                        -T assign_t -T config_t -T config_setting_T \
                        -T DIR -T DIRP $^
