#:ts=8
#
#    $Id: Makefile,v 4.5 1997/08/20 21:05:01 mj Exp $
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


SUBDIRS		= cnews filebase inn maintenance misc nodelist outbound run


all clean depend::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@; done 

install::
	@echo "***** make install NOT done for scripts subdirs *****"
	@echo "Use make install in the individual subdirs!"

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:
