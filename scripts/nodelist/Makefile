#:ts=8
#
#    $Id: Makefile,v 4.4 1997/07/25 21:01:35 mj Exp $
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
		  nl-autoupd nl-del nl-diff nl-html nl-isdn nl-print\
		  nl-2hosts nl-2routing


all:		$(PROGS)



install:
	$(INSTALL_PROG)   latest      $(BINDIR)
	$(INSTALL_PROG)   nl-autoupd  $(BINDIR)
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
