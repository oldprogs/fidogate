#:ts=8
#
#    $Id: Makefile,v 4.4 1997/08/20 21:05:02 mj Exp $
#
#    Makefile for FIDOGATE src/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


SUBDIRS		= include common ffx gate tick toss util


all clean veryclean depend install check::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@ || exit 1; done 

clean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check::
