#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES 		= @INCLUDES@ -I$(top_srcdir)/src -I$(top_srcdir)
MEXFLAGS 		= @MEXFLAGS@ -inline $(INCLUDES) -v \
           	 	  CC='$(CC)' LD='$(CC) -fPIC $(CFLAGS)' \
              		  CFLAGS='-fPIC -D_GNU_SOURCE -pthread $(CFLAGS)'
MEXLDADD		= $(top_builddir)/src/libmalheur.a $(LIBS)

EXTRA_DIST		= malheur.m malheur.c mal_load_mist.m \
			  mal_distance.m malmex.c malmex.h \
			  mal_prototype.m mal_dot_product.m

if MATLAB_TOOLS
matlab_DATA             = malheur.m mal_load_mist.m mal_distance.m \
			  mal_prototype.m mal_dot_product.m
matlabarch_SCRIPTS      = malheur.$(MEXEXT)
endif

malheur_SRC		= malheur.c malmex.c

malheur.$(MEXEXT): $(malheur_SRC)
	mkdir -p .libs/$@ ; \
	$(MEX) $(MEXOPTS) $(MEXFLAGS) -outdir .libs/$@ $^ $(MEXLDADD) ; \
	mv .libs/$@/$@ . ; rm -rf .libs/$@

mostlyclean-local:
	rm -f *.$(MEXEXT)
        