#:ts=8
#
#    $Id: Makefile,v 4.0 1996/04/17 18:17:34 mj Exp $
#
#    Makefile for fidogate software, lib/areas directory
#
#
SHELL	= /bin/sh


AREAS	= local oecher gated fido.ger fido.int sysop restr\
	  usenet.single usenet.complete

areas:	HEAD $(AREAS)
	cat HEAD $(AREAS) > ../areas

clean:
	rm -f *.bak *~
