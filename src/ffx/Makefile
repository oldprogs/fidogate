#:ts=8
#
#    $Id: Makefile,v 4.2 1997/07/25 21:01:40 mj Exp $
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
	$(INSTALL_SETUID) ffx$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ffxqt$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ffxbatch$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ffxmail $(LIBDIR)
	$(INSTALL_PROG) ffxrmail $(LIBDIR)
	$(INSTALL_PROG) ffxrun $(LIBDIR)


clean:
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
