#:ts=8
#
#    $Id: Makefile,v 4.3 1998/11/15 10:59:03 mj Exp $
#
#    Makefile for FIDOGATE scripts/filebase/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= areasbbssync recvuu senduu senduumail


all:		$(PROGS)


SUBSTCMD	= $(PERL) $(TOPDIR)/subst.pl -t$(TOPDIR) \
		  -c$(TOPDIR)/examples/morannon/config.make.morannon -p

morannon:
	$(SUBSTCMD) areasbbssync.pl >areasbbssync
	chmod +x areasbbssync


install:
	$(INSTALL_PROG) areasbbssync $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) recvuu $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) senduu $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) senduumail $(PREFIX)$(LIBDIR)


clean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Dependencies
#
depend:

depend-all:

ifeq (.depend,$(wildcard .depend))
include .depend
endif
