#:ts=8
#
#    $Id: Makefile,v 4.2 1996/06/09 10:49:00 mj Exp $
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


all clean depend::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@; done 

install::
	@echo "***** make install NOT done for scripts subdirs *****"
	@echo "Use make install in the individual subdirs!"

clean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:
