#:ts=8
#
#    $Id: Makefile,v 4.3 1997/08/20 21:05:04 mj Exp $
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


PROGS		= ftnhatch ftntick


all:		$(PROGS)

ftnhatch:	$(TOPDIR)/src/common/$(LIB)
ftntick:	$(TOPDIR)/src/common/$(LIB)


install:
	$(INSTALL_PROG) ftntick$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ftnhatch$(EXE) $(LIBDIR)


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
