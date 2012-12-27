# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009-2012 Konrad Rieck (konrad@mlsec.org)
# University of Goettingen, Berlin Institute of Technology
# --

AM_CPPFLAGS 		= @AM_CPPFLAGS@ -I$(top_srcdir)/src -I$(top_srcdir)
MEXFLAGS 		= @MEXFLAGS@ -inline $(AM_CPPFLAGS) -v \
           	 	  CC='$(CC)' LD='$(CC) -fPIC $(CFLAGS)' \
              		  CFLAGS='-fPIC -D_GNU_SOURCE -pthread $(CFLAGS)'
MEXLDADD		= $(top_builddir)/src/libmalheur.a $(LIBS)

EXTRA_DIST		= malheur.m malheur.c \
			  mal_distance.m malmex.c malmex.h

if MATLAB_TOOLS
matlab_DATA             = malheur.m mal_distance.m 
matlabarch_SCRIPTS      = malheur.$(MEXEXT)
endif

malheur_SRC		= malheur.c malmex.c

malheur.$(MEXEXT): $(malheur_SRC)
	mkdir -p .libs/$@ ; \
	$(MEX) $(MEXOPTS) $(MEXFLAGS) -outdir .libs/$@ $^ $(MEXLDADD) ; \
	mv .libs/$@/$@ . ; rm -rf .libs/$@

mostlyclean-local:
	rm -f *.$(MEXEXT)
        
