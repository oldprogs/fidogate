#:ts=8
#
#    $Id: Makefile,v 4.7 1998/11/08 18:27:59 mj Exp $
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


SCRIPTS		= ffxmail ffxnews ffxrmail runffx send-ffx
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
	$(INSTALL_PROG) runffx $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) ffxnews $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) send-ffx $(PREFIX)$(NEWSETCDIR)


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
