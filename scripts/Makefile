#:ts=8
#
#    $Id: Makefile,v 4.1 1996/05/03 19:18:42 mj Exp $
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


SUBDIRS		= filebase maintenance misc nodelist outbound run


all clean depend install::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@; done 

clean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:
