#:ts=8
#
#    $Id: Makefile,v 4.0 1996/04/17 18:17:42 mj Exp $
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
	$(INSTALL_PROG)   ftntick     $(LIBDIR)
	$(INSTALL_PROG)   ftnhatch    $(LIBDIR)


clean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


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
