#
# $Id: Makefile,v 4.6 1997/08/10 17:34:19 mj Exp $
#
# Makefile FIDOGATE TOPDIR
#

TOPDIR		= .

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIRS		= src scripts test doc sendmail


all clean veryclean check depend install::
	for d in $(SUBDIRS); do \
	  if [ -f $$d/Makefile ]; then $(MAKE) -C $$d $@ || exit 1; fi; \
	done

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core

install-dirs:
	$(INSTALL_DIR)    $(LIBDIR)
	$(INSTALL_DIR)    $(SPOOLDIR)
	$(INSTALL_DIR)    $(SPOOLDIR)/in
	$(INSTALL_DIR)    $(SPOOLDIR)/in/tmpmail
	$(INSTALL_DIR)    $(SPOOLDIR)/in/tmpnews
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

install::
	cp ANNOUNCE $(HTMLDIR)
