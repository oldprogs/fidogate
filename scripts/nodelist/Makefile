#:ts=8
#
#    $Id: Makefile,v 4.3 1997/06/21 21:16:41 mj Exp $
#
#    Makefile for FIDOGATE scripts/filebase/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= latest\
		  nl-del nl-diff nl-html nl-isdn nl-print\
		  nl-2hosts nl-2routing


all:		$(PROGS)



install:
	$(INSTALL_PROG)   nl-del      $(BINDIR)
	$(INSTALL_PROG)   nl-diff     $(BINDIR)


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
