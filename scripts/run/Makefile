#:ts=8
#
#    $Id: Makefile,v 4.4 1998/01/28 22:00:13 mj Exp $
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


PROGS		= run1 run2 runpoll runffx


all:		$(PROGS)



install:
	$(INSTALL_PROG) run1 $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) run2 $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) runpoll $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) runffx $(PREFIX)$(BINDIR)


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
