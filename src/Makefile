#:ts=8
#
#    $Id: Makefile,v 4.9 1998/02/25 09:15:24 mj Exp $
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


SUBDIRS		= include common areafix ffx gate tick toss util


all::

install::


all clean veryclean depend install check::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@ || exit 1; done 

clean::
	rm -f *~ *.bak *.o tags TAGS core config.pl

check::

tags:
	etags */*.[hcy] */*.pl *.pl
