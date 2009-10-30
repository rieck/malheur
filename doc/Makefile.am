#
# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#

sysconf_DATA = malheur.cfg
man1_MANS = malheur.man 
EXTRA_DIST = malheur.cfg malheur.man.in doxygen.cfg modules

malheur.man: malheur.man.in
	sed -e 's|__VERSION__|$(VERSION)|' \
        	$(top_srcdir)/doc/malheur.man.in > malheur.man

clean-local:
	rm -f malheur.man
        
        