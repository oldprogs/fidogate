#:ts=8
#
#    $Id: Makefile,v 4.5 1999/01/09 17:39:40 mj Exp $
#
#    Makefile for FIDOGATE scripts/run/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= runpoll runpoll-orodruin


all:		$(PROGS)



install:
	$(INSTALL_PROG) runpoll $(PREFIX)$(BINDIR)


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
