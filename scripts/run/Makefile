#:ts=8
#
#    $Id: Makefile,v 4.1 1996/09/22 12:26:47 mj Exp $
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


PROGS		= run1 run2 runpoll


all:		$(PROGS)



install:
	$(INSTALL_PROG)   run1        $(LIBDIR)
	$(INSTALL_PROG)   run2        $(LIBDIR)
	$(INSTALL_PROG)   runpoll     $(LIBDIR)


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
