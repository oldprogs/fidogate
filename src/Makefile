#:ts=8
#
#    $Id: Makefile,v 4.0 1996/04/17 18:17:38 mj Exp $
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


SUBDIRS		= include common ffx gate tick toss util


all clean depend install::
	for d in $(SUBDIRS); do $(MAKE) -C $$d $@; done 

install-dirs:
	$(INSTALL_DIR)    $(LIBDIR)
	$(INSTALL_DIR)    $(SPOOLDIR)
	$(INSTALL_DIR)    $(SPOOLDIR)/in
	$(INSTALL_DIR)    $(SPOOLDIR)/in/mail
	$(INSTALL_DIR)    $(SPOOLDIR)/in/news
	$(INSTALL_DIR)    $(SPOOLDIR)/in/bad
	$(INSTALL_DIR)    $(SPOOLDIR)/insecure
	$(INSTALL_DIR)    $(SPOOLDIR)/out
	$(INSTALL_DIR)    $(SPOOLDIR)/locks
	$(INSTALL_DIR)    $(SPOOLDIR)/seq
	$(INSTALL_DIR)    $(SPOOLDIR)/toss
	$(INSTALL_DIR)    $(SPOOLDIR)/toss/in
	$(INSTALL_DIR)    $(SPOOLDIR)/toss/bad
	$(INSTALL_DIR)    $(SPOOLDIR)/toss/tmp
	$(INSTALL_DIR)    $(SPOOLDIR)/toss/out
	$(INSTALL_DIR)    $(SPOOLDIR)/toss/pack
	$(INSTALL_DIR)    $(LOGDIR)
	$(INSTALL_DIR)    $(OUTBOUND)
	$(INSTALL_DIR)    $(INBOUND)
	$(INSTALL_DIR)    $(PINBOUND)
	$(INSTALL_DIR)    $(UUINBOUND)

clean::
	rm -f *~ *.bak *.o tags TAGS core paths.tmp
