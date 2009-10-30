#
# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009 Konrad Rieck (rieck@cs.tu-berlin.de)
# Berlin Institute of Technology (TU Berlin).
#

sysconf_DATA = malheur.cfg
man1_MANS = malheur.man 
EXTRA_DIST = malheur.cfg malheur.pod doxygen.cfg modules

%.man: %.pod
	pod2man --center "User Manual" --section=1 \
	        --release="malheur $(VERSION)" $< > $@
        
clean-local:
	rm -f malheur.man
        
        