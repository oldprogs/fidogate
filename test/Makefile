#:ts=8
#
#    $Id: Makefile,v 4.0 1996/04/17 18:17:36 mj Exp $
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


SUBDIRS		= tc.gate1 tc.gate2\
		  tc.toss1 tc.toss2 tc.toss3 tc.toss4


all depend install::

clean::
	for d in $(SUBDIRS); do (echo "$$d"; cd $$d; ./clean); done 

clean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp
