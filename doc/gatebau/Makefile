#
# $Id: Makefile,v 1.2 1996/12/29 12:16:37 mj Exp $
#
# Makefile FIDOGATE doc/gatebau/
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


all:		msgid.html msgid.tex msgid.txt


msgid.html:	msgid.sgml
	-sgml2html msgid.sgml

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
