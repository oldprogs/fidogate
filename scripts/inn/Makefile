#:ts=8
#
#    $Id: Makefile,v 1.2 1997/10/11 21:24:23 mj Exp $
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
	$(INSTALL_PROG) send-fidogate $(PREFIX)$(NEWSETCDIR)
	$(INSTALL_PROG) send-ffx      $(PREFIX)$(NEWSETCDIR)
	$(INSTALL_PROG) viaffx        $(PREFIX)$(NEWSETCDIR)


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
