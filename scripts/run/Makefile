#:ts=8
#
#    $Id: Makefile,v 4.3 1997/11/16 15:53:24 mj Exp $
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
	$(INSTALL_PROG) run1 $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) run2 $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) runpoll $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) runffx $(PREFIX)$(LIBDIR)


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
