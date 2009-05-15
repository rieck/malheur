#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#


noinst_LIBRARIES	= libmalheur.a			  
libmalheur_a_SOURCES	= md5.c md5.h common.h util.c util.h fvec.c \
                          fvec.h uthash.h fhash.c fhash.h mist.c mist.h \
                          fmath.c fmath.h

bin_PROGRAMS		= malheur
malheur_SOURCES		= $(libmalheur_a_SOURCES) malheur.c malheur.h
malheur_LDFLAGS		= -static

beautify:       	$(libmalheur_a_SOURCES) $(malheur_SOURCES)
			gnuindent -i4 -kr -l77 -lc77 --no-tabs -cs \
				  -T FILE -T fvet_t -T DIR -T DIRP $^
