#
# $Id: Makefile,v 4.0 1996/04/17 18:17:32 mj Exp $
#
# Makefile FIDOGATE TOPDIR
#

TOPDIR		= .

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIRS		= src scripts test doc sendmail


all clean veryclean::
	for d in $(SUBDIRS); do \
	  if [ -f $$d/Makefile ]; then (cd $$d; make $@); fi; \
	done

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core
