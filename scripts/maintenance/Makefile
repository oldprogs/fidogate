#:ts=8
#
#    $Id: Makefile,v 4.5 1997/10/11 21:24:24 mj Exp $
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


PROGS		= logmaint logreport logstat sendmailstat\
		  newsyslog rc.fidogate


all:		$(PROGS)



install:
	$(INSTALL_PROG)   newsyslog       $(PREFIX)$(BINDIR)
	$(INSTALL_PROG)   sendmailstat    $(PREFIX)$(BINDIR)
	$(INSTALL_PROG)   logmaint        $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG)   logreport       $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG)   logstat         $(PREFIX)$(LIBDIR)


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
