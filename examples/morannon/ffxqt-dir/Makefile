#:ts=8
#
#    $Id: Makefile,v 1.1 1996/10/03 09:51:24 mj Exp $
#
#    Makefile for fidogate software, lib/ffxqt directory
#

TOPDIR		= ../../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


FFXQTDIR	= $(LIBDIR)/ffxqt-dir

PROG		= gunzip unzip unbatchzip rmail
CMD		= rnews


all:		$(PROG) $(CMD)


rmail:
	rm -f rmail
	ln -s $(TOPDIR)/src/ffxrmail rmail

rnews:
	rm -f rnews
	ln -s /usr/bin/rnews .

clean:
	rm -f $(PROG) $(CMD)
	rm -f core *~ *.bak *.o tags TAGS


install:
	if [ ! -d $(LIBDIR)   ]; then mkdir $(LIBDIR)  ; fi
	if [ ! -d $(FFXQTDIR) ]; then mkdir $(FFXQTDIR); fi
	for f in $(PROG); do\
		rm -f $(FFXQTDIR)/$$f;\
		$(INSTALL_PROG) $$f $(FFXQTDIR);\
	done
	for f in $(CMD); do\
		(cd $(FFXQTDIR); rm -f $$f; ln -s /usr/bin/$$f .)\
	done
