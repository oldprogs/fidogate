#:ts=8
#
#    $Id: Makefile,v 4.11 1999/10/17 11:49:29 mj Exp $
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


PROGS		= logcheck logreport logstat logdaily logsendmail\
		  rc.fidogate runchklock


all:		$(PROGS)
$(PROGS):	$(TOPDIR)/config.make $(TOPDIR)/config.pl



install:
	$(INSTALL_PROG) logsendmail $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logreport $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logstat $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logdaily $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logcheck $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) runchklock $(PREFIX)$(BINDIR)


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
