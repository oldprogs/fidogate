#:ts=8
#
#    $Id: Makefile,v 4.5 1999/06/27 19:18:13 mj Exp $
#
#    Makefile for FIDOGATE src/tick/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= ftnhatch ftntick ftntickpost


all:		$(PROGS)

ftnhatch:	$(TOPDIR)/src/common/$(LIB)
ftntick:	$(TOPDIR)/src/common/$(LIB)


install:
	$(INSTALL_PROG) ftntick$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftntickoist $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) ftnhatch$(EXE) $(PREFIX)$(LIBDIR)


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
