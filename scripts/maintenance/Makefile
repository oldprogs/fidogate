#:ts=8
#
#    $Id: Makefile,v 4.3 1997/04/19 11:41:47 mj Exp $
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


PROGS		= logmaint logreport logstat\
		  newsyslog rc.fidonet sendmailstat


all:		$(PROGS)



install:
	$(INSTALL_PROG)   newsyslog       $(BINDIR)
	$(INSTALL_PROG)   sendmailstat    $(BINDIR)
	$(INSTALL_PROG)   logmaint        $(LIBDIR)
	$(INSTALL_PROG)   logreport       $(LIBDIR)
	$(INSTALL_PROG)   logstat         $(LIBDIR)


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
