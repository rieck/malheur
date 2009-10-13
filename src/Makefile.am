#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#

BUILT_SOURCES		= malheur.h
EXTRA_DIST              = malheur.h.in
noinst_LIBRARIES        = libmalheur.a			  
libmalheur_a_SOURCES	= md5.c md5.h common.h util.c util.h fvec.c \
                          fvec.h uthash.h ftable.c ftable.h mist.c \
                          mist.h fmath.c fmath.h export.c export.h \
                          farray.c farray.h proto.c proto.h mconfig.c \
                          mconfig.h cluster.c cluster.h quality.c \
                          quality.h classify.c classify.h 

bin_PROGRAMS            = malheur 
malheur_DEPENDENCIES	= malheur.h
malheur_SOURCES    	= $(libmalheur_a_SOURCES) malheur.c
malheur_LDFLAGS         = -static

malheur.h: 		malheur.h.in
			sed -e 's|__SYSCONFDIR__|$(sysconfdir)|' \
    -e 's|__LOCALSTATEDIR__|$(localstatedir)|' \
    $(top_srcdir)/src/malheur.h.in > malheur.h

clean-local:
	rm -f malheur.h
        
beautify:               $(libmalheur_a_SOURCES) $(malheur_SOURCES)
			gnuindent -i4 -kr -l77 -lc77 --no-tabs -cs \
                        -T FILE -T fvec_t -T farray_t -T cluster_t \
                        -T assign_t -T config_t -T config_setting_T \
                        -T DIR -T DIRP $^
