#:ts=8
#
#    $Id: Makefile,v 4.6 2000/06/03 20:36:40 mj Exp $
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


PROGS		= areasbbssync recvuu senduu senduumail hosts2dns


all:		$(PROGS)
$(PROGS):	$(TOPDIR)/config.make $(TOPDIR)/config.pl


SUBSTCMD	= $(PERL) $(TOPDIR)/subst.pl -t$(TOPDIR) \
		  -c$(TOPDIR)/examples/morannon/config.make.morannon -p

morannon:
	$(SUBSTCMD) areasbbssync.pl >areasbbssync
	chmod +x areasbbssync


install:
	$(INSTALL_PROG) areasbbssync $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) recvuu $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) senduu $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) senduumail $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) hosts2dns $(PREFIX)$(BINDIR)


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
