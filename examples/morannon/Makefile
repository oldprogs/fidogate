#:ts=8
#
#    $Id: Makefile,v 4.0 1996/04/17 18:17:33 mj Exp $
#
#    Makefile for fidogate software, lib directory
#

TOPDIR		= ..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


LIBFILES	= aliases hosts areas\
		  bounce.restricted bounce.noto bounce.unsecure bounce.down\
		  config.common config.gate config.main config.ffx\
		  routing packing

COMMON		= aliases areas hosts maus routing packing


all:	areas


install:
	$(INSTALL_DATA) $(LIBFILES) $(LIBDIR)

install-common:
	$(INSTALL_DATA) $(COMMON) $(LIBDIR)

admin-hosts:
	mv hosts hosts.bak
	cp hosts.head hosts
	admin -l hosts >>hosts

clean:
	rm -f core *~ *.bak *.o tags TAGS
