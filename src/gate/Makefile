#:ts=8
#
#    $Id: Makefile,v 4.7 1998/03/07 16:53:29 mj Exp $
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


PROGS		= ftn2rfc ftnin rfc2ftn ftnmail
SCRIPTS		= ftninpost ftninrecomb


all:		$(PROGS) $(SCRIPTS)

$(SCRIPTS):	$(TOPDIR)/config.pl

ftn2rfc:	$(TOPDIR)/src/common/$(LIB)
ftnin:		$(TOPDIR)/src/common/$(LIB)
rfc2ftn:	$(TOPDIR)/src/common/$(LIB)
ftnamil:	$(TOPDIR)/src/common/$(LIB)


install:
	$(INSTALL_SETUID) ftnmail$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) rfc2ftn$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftn2rfc$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftnin$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftninpost $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftninrecomb $(PREFIX)$(LIBDIR)


clean veryclean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:


#
# Dependencies
#
*.pl:	$(TOPDIR)/config.pl

depend:
	$(CC) $(CFLAGS) -MM *.c >.depend

depend-all:
	$(CC) $(CFLAGS) -M *.c >.depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
