#:ts=8
#
#    $Id: Makefile,v 4.5 1997/07/25 21:01:43 mj Exp $
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
	$(INSTALL_PROG) ftnbsy$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ftnconfig$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ftnseq$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ftnfattach$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ftnflo$(EXE) $(LIBDIR)
	$(INSTALL_PROG) sumcrc$(EXE) $(BINDIR)
	$(INSTALL_PROG) ftnlock$(EXE) $(LIBDIR)
	$(INSTALL_PROG) ftnlog$(EXE) $(LIBDIR)
	$(INSTALL_PROG) pktdebug$(EXE) $(BINDIR)
	$(INSTALL_PROG) pktmore $(BINDIR)


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
