#:ts=8
#
#    $Id: Makefile,v 4.4 1997/10/11 21:24:27 mj Exp $
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


PROGS		= ffx ffxqt ffxbatch ffxmail ffxrmail ffxrun


all:		$(PROGS)

ffx:		$(TOPDIR)/src/common/$(LIB)
ffxqt:		$(TOPDIR)/src/common/$(LIB)
ffxbatch:	$(TOPDIR)/src/common/$(LIB)


install:
	$(INSTALL_SETUID) ffx$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxqt$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxbatch$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxmail $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxrmail $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ffxrun $(PREFIX)$(LIBDIR)


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
