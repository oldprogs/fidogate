#:ts=8
#
#    $Id: Makefile,v 4.1 1999/07/18 15:00:22 mj Exp $
#
#    Makefile for FIDOGATE sendmail/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIR		= cf


all:		README make-cf


README:		$(SENDMAIL_CF_DIR)/README
	./mklinks $(SENDMAIL_CF_DIR)

make-cf:

install::

clean::
	./mkclean
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

clean veryclean check test verify depend install::
	make -C $(SUBDIR) $@

check::

depend::

depend-all::
