#:ts=8
#
#    $Id: Makefile,v 1.1 1998/11/15 10:59:04 mj Exp $
#
#    Makefile for FIDOGATE scripts/www/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= gate.cgi


all:		$(PROGS)



install:
	$(INSTALL_PROG) gate.cgi $(PREFIX)$(HTMLLOGDIR)


clean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Dependencies
#
depend:

depend-all:

ifeq (.depend,$(wildcard .depend))
include .depend
endif
