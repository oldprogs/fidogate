#:ts=8
#
#    $Id: Makefile,v 4.3 1999/05/15 20:54:36 mj Exp $
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


PROGS		= out-freq out-ls out-manip out-rm0 out-rmbsy


all:		$(PROGS)
$(PROGS):	$(TOPDIR)/config.make $(TOPDIR)/config.pl



install:
	$(INSTALL_PROG) out-manip $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) out-rm0 $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) out-rmbsy $(PREFIX)$(BINDIR)


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
