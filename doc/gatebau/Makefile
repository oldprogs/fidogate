#
# $Id: Makefile,v 1.3 1997/05/01 17:49:49 mj Exp $
#
# Makefile FIDOGATE doc/gatebau/
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


all:		msgid.html msgid.tex msgid.txt


msgid.html:	msgid.sgml
	-sgml2html msgid.sgml
	./post-html.pl msgid*.html

msgid.tex:	msgid.sgml
	-sgml2latex -2e -a msgid.sgml

msgid.txt:	msgid.sgml
	-sgml2txt msgid.sgml


clean:
	rm -f *~ *.bak *.o tags TAGS core

veryclean:	clean
	rm -f *.html *.tex *.txt


check:

install:
	cp msgid*.html $(HTMLDIR)

depend:
