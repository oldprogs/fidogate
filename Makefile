#
# $Id: Makefile,v 4.7 1997/08/20 21:04:57 mj Exp $
#
# Makefile FIDOGATE TOPDIR
#

TOPDIR		= .

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIRS		= src scripts test doc sendmail

INSTALLDIRS	= $(LIBDIR) $(SPOOLDIR) $(SPOOLDIR)/in \
		  $(SPOOLDIR)/in/tmpmail $(SPOOLDIR)/in/tmpnews \
		  $(SPOOLDIR)/in/bad $(SPOOLDIR)/insecure \
		  $(SPOOLDIR)/out $(SPOOLDIR)/locks $(SPOOLDIR)/seq \
		  $(SPOOLDIR)/toss $(SPOOLDIR)/toss/in \
		  $(SPOOLDIR)/toss/bad $(SPOOLDIR)/toss/tmp \
		  $(SPOOLDIR)/toss/out $(SPOOLDIR)/toss/pack $(LOGDIR) \
		  $(OUTBOUND) $(INBOUND) $(PINBOUND) $(UUINBOUND) \
		  $(BINDIR) $(INFODIR)



all clean veryclean check depend install::
	for d in $(SUBDIRS); do \
	  if [ -f $$d/Makefile ]; then $(MAKE) -C $$d $@ || exit 1; fi; \
	done

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core

install-dirs:
	for d in $(INSTALLDIRS); do if [ ! -d $$d ]; then \
	    echo "Creating $$d ..."; $(INSTALL_DIR) $$d; \
	fi; done

install::
	cp ANNOUNCE $(HTMLDIR)
