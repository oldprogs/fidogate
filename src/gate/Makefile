#:ts=8
#
#    $Id: Makefile,v 4.1 1996/05/03 19:18:44 mj Exp $
#
#    Makefile for FIDOGATE src/gate/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= ftn2rfc ftnin ftninpost ftninrecomb rfc2ftn


all:		$(PROGS)

ftn2rfc:	$(TOPDIR)/src/common/$(LIB)
ftnin:		$(TOPDIR)/src/common/$(LIB)
rfc2ftn:	$(TOPDIR)/src/common/$(LIB)


install:
	$(INSTALL_SETUID)  rfc2ftn     $(LIBDIR)
	$(INSTALL_PROG)   ftn2rfc     $(LIBDIR)
	$(INSTALL_PROG)   ftnin       $(LIBDIR)
	$(INSTALL_PROG)   ftninpost   $(LIBDIR)
	$(INSTALL_PROG)   ftninrecomb $(LIBDIR)


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
