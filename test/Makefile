#:ts=8
#
#    $Id: Makefile,v 4.5 1998/03/07 16:53:32 mj Exp $
#
#    Makefile for FIDOGATE test/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


all depend install::

all::
	(cd lib; ./mklinks)

check run::
	for d in tc.*; do (echo "$$d"; cd $$d; ./run) || exit 1; done 


clean::
	for d in tc.* tmp*; do (echo "$$d"; cd $$d; ../tr.clean); done 

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

verify::
	for d in tc.*; do \
	  if [ -f $$d/run.verify ]; then \
	    (cd $$d; ../tr.verify $$d) \
          fi; \
	done 
