#:ts=8
#
#    $Id: Makefile,v 4.1 1996/04/22 14:28:48 mj Exp $
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

check run::
	for d in tc.*; do (echo "$$d"; cd $$d; ./run) || exit 1; done 


clean::
	for d in tc.*; do (echo "$$d"; cd $$d; ../tr.clean); done 

clean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp
