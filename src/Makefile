#:ts=8
#
#    $Id: Makefile,v 4.7 1998/02/04 19:17:36 mj Exp $
#
#    Makefile for FIDOGATE src/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


SUBDIRS		= include common ffx gate tick toss util


all::		config.pl

config.pl:	config.plt $(TOPDIR)/config.make $(TOPDIR)/subst.pl
	$(PERL) $(TOPDIR)/subst.pl -c$(TOPDIR)/config.make -p $< >$*.pl


install::
	$(INSTALL_PROG) config.pl $(PREFIX)$(LIBDIR)


all clean veryclean depend install check::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@ || exit 1; done 

clean::
	rm -f *~ *.bak *.o tags TAGS core config.pl

check::

tags:
	etags */*.[hcy] */*.pl *.pl
