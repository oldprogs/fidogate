#:ts=8
#
#    $Id: Makefile,v 4.1 1996/12/29 12:16:38 mj Exp $
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


PROGS		= afmail areamgr recvuu senduu senduumail


all:		$(PROGS)



install:


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
