#
# $Id: Makefile,v 1.2 1996/10/06 15:17:36 mj Exp $
#
# Makefile FIDOGATE doc/
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


all:		fidogate_toc.html


fidogate_toc.html:	../fidogate.texi
	-texi2html -verbose -menu -split_chapter ../fidogate.texi


clean:
	rm -f *~ *.bak *.o tags TAGS core

veryclean:	clean
	rm -f fidogate_*.html


check:

install:
	$(INSTALL_DATA) fidogate_*.html $(HTMLDIR)

depend:
