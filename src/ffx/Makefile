#:ts=8
#
#    $Id: Makefile,v 4.6 1998/02/25 09:15:26 mj Exp $
#
#    Makefile for FIDOGATE src/ffx/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


SCRIPTS		= ffxmail ffxrmail
PROGS		= ffx ffxqt $(SCRIPTS)


all:		$(PROGS)

ffx:		$(TOPDIR)/src/common/$(LIB)
ffxqt:		$(TOPDIR)/src/common/$(LIB)
ffxbatch:	$(TOPDIR)/src/common/$(LIB)

$(SCRIPTS):	$(TOPDIR)/subst.pl $(TOPDIR)/config.pl


install:
	$(INSTALL_SETUID) ffx$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxqt$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxmail $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxrmail $(PREFIX)$(LIBDIR)


clean veryclean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:


#
# Dependencies
#
depend:
	$(CC) $(CFLAGS) -MM *.c >.depend

depend-all:
	$(CC) $(CFLAGS) -M *.c >.depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
