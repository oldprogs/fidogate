#:ts=8
#
#    $Id: Makefile,v 4.7 1999/03/06 17:51:24 mj Exp $
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
	$(INSTALL_PROG) runpoll          $(PREFIX)$(BINDIR)

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
