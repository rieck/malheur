#
# MALHEUR - Automatic Malware Analysis on Steroids
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
# --
#

INCLUDES 		= @INCLUDES@ -I$(top_srcdir)/src -I$(top_srcdir)
MEXFLAGS 		= @MEXFLAGS@ -inline $(INCLUDES) -v \
           	 	  CC='$(CC)' LD='$(CC) $(CFLAGS) -fPIC $(LIBS)' \
              		  CFLAGS='-fPIC -D_GNU_SOURCE -pthread $(CFLAGS)'
MEXLDADD		= $(top_builddir)/src/libmalheur.a              		  

EXTRA_DIST		= malheur.m malheur.c malheur_load.m 

if MATLAB_TOOLS
matlab_DATA             = malheur.m malheur_load.m
matlabarch_SCRIPTS      = malheur.$(MEXEXT)
endif

malheur_SRC		= malheur.c 

malheur.$(MEXEXT): $(malheur_SRC)
	mkdir -p .libs/$@ ; \
	$(MEX) $(MEXOPTS) $(MEXFLAGS) -outdir .libs/$@ $^ $(MEXLDADD) ; \
	mv .libs/$@/$@ . ; rm -rf .libs/$@

mostlyclean-local:
	rm -f *.$(MEXEXT)
        