#:ts=8
#
#    $Id: Makefile,v 4.6 1999/02/07 11:05:22 mj Exp $
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


PROGS		= runpoll runpoll-orodruin runpoll-binkd


all:		$(PROGS)



install:
	$(INSTALL_PROG) $(PROGS) $(PREFIX)$(BINDIR)

install-orodruin:
	$(INSTALL_PROG) runpoll-orodruin $(PREFIX)$(BINDIR)/runpoll
	$(INSTALL_PROG) runpoll-binkd    $(PREFIX)$(BINDIR)/runpolli


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
