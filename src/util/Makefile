#:ts=8
#
#    $Id: Makefile,v 4.4 1996/12/29 12:16:39 mj Exp $
#
#    Makefile for FIDOGATE src/util/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= ftnfattach ftnbsy ftnconfig ftnlock ftnlog ftnseq ftnflo\
		  pktdebug pktmore sumcrc


all:		$(PROGS)

ftnbsy:		$(TOPDIR)/src/common/$(LIB)
ftnconfig:	$(TOPDIR)/src/common/$(LIB)
ftnfattach:	$(TOPDIR)/src/common/$(LIB)
ftnflo:		$(TOPDIR)/src/common/$(LIB)
ftnlock:	$(TOPDIR)/src/common/$(LIB)
ftnlog:		$(TOPDIR)/src/common/$(LIB)
ftnseq:		$(TOPDIR)/src/common/$(LIB)
pktdebug:	$(TOPDIR)/src/common/$(LIB)
sumcrc:		$(TOPDIR)/src/common/$(LIB)


install:
	$(INSTALL_PROG)   ftnbsy      $(LIBDIR)
	$(INSTALL_PROG)   ftnconfig   $(LIBDIR)
	$(INSTALL_PROG)   ftnseq      $(LIBDIR)
	$(INSTALL_PROG)   ftnfattach  $(LIBDIR)
	$(INSTALL_PROG)   ftnflo      $(LIBDIR)
	$(INSTALL_PROG)   sumcrc      $(BINDIR)
	$(INSTALL_PROG)   ftnlock     $(LIBDIR)
	$(INSTALL_PROG)   ftnlog      $(LIBDIR)
	$(INSTALL_PROG)   pktdebug    $(BINDIR)
	$(INSTALL_PROG)   pktmore     $(BINDIR)


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
