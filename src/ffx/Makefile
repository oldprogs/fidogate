#:ts=8
#
#    $Id: Makefile,v 4.5 1998/01/24 15:45:53 mj Exp $
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


PROGS		= ffx ffxqt ffxmail ffxrmail


all:		$(PROGS)

ffx:		$(TOPDIR)/src/common/$(LIB)
ffxqt:		$(TOPDIR)/src/common/$(LIB)
ffxbatch:	$(TOPDIR)/src/common/$(LIB)


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
