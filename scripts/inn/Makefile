#:ts=8
#
#    $Id: Makefile,v 1.1 1996/09/28 08:13:52 mj Exp $
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


PROGS		= send-fidogate send-ffx viaffx


all:		$(PROGS)



install:
	$(INSTALL_PROG) send-fidogate $(NEWSETCDIR)
	$(INSTALL_PROG) send-ffx      $(NEWSETCDIR)
	$(INSTALL_PROG) viaffx        $(NEWSETCDIR)


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
