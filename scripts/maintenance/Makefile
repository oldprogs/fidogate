#:ts=8
#
#    $Id: Makefile,v 4.2 1997/02/09 10:04:22 mj Exp $
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


PROGS		= logmaint logreport logstat newsyslog rc.fidonet sendmailstat


all:		$(PROGS)



install:
	$(INSTALL_PROG)   newsyslog       $(BINDIR)
	$(INSTALL_PROG)   sendmailstat    $(BINDIR)


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
