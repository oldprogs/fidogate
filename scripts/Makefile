#:ts=8
#
#    $Id: Makefile,v 4.9 2000/01/30 21:55:04 mj Exp $
#
#    Makefile for FIDOGATE scripts/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


SUBDIRS		= filebase maintenance misc nodelist outbound run www


all clean veryclean depend install::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@; done 

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:
verify:	check
test:	check
