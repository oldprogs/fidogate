#
# $Id: Makefile,v 1.4 1996/12/22 10:39:35 mj Exp $
#
# Makefile FIDOGATE doc/html/
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
	cp fidogate_*.html $(HTMLDIR)

depend:
