#:ts=8
#
#    $Id: Makefile,v 4.11 2004/08/22 10:30:04 n0ll Exp $
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

test verify::
	@for d in tc.*; do \
	  if [ -f $$d/run.verify ]; then \
	    (cd $$d; ../tr.verify $$d) \
	  fi; \
	done 

all-mk-verify::
	@for d in tc.*; do \
	  if [ -f $$d/run.verify ]; then \
	    (echo $$d; cd $$d; ../mk.verify $$d) \
	  fi; \
	done 
