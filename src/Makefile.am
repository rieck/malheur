#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#

bin_PROGRAMS		= malheur
malheur_SOURCES		= malheur.c malheur.h md5.c md5.h common.h \
			  util.c util.h fvec.c fvec.h uthash.h \
			  fhash.c fhash.h mist.c mist.h fmath.c fmath.h
malheur_LDFLAGS		= -static

beautify:       	$(malheur_SOURCES)
			gnuindent -i4 -kr -l77 -lc77 --no-tabs -cs \
				  -T FILE -T fvet_t -T DIR -T DIRP $^
