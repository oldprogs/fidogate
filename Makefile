#
# $Id: Makefile,v 4.2 1996/04/24 12:15:27 mj Exp $
#
# Makefile FIDOGATE TOPDIR
#

TOPDIR		= .

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIRS		= src scripts test doc sendmail


all clean veryclean check depend::
	for d in $(SUBDIRS); do \
	  if [ -f $$d/Makefile ]; then $(MAKE) -C $$d $@ || exit 1; fi; \
	done

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core
