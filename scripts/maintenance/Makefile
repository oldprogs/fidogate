#:ts=8
#
#    $Id: Makefile,v 4.8 1998/11/08 18:27:56 mj Exp $
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
		  rc.fidogate


all:		$(PROGS)



install:
	$(INSTALL_PROG) logsendmail $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logmaint $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logreport $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logstat $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) logdaily $(PREFIX)$(BINDIR)


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
