#
# $Id: Makefile,v 4.12 1998/01/18 17:49:05 mj Exp $
#
# Makefile FIDOGATE TOPDIR
#

TOPDIR		= .

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIRS		= src scripts test doc sendmail

INSTALLDIRS	= $(DEFAULT_CONFIGDIR) \
		  $(DEFAULT_LIBDIR) \
		  $(DEFAULT_BINDIR) \
		  $(DEFAULT_LOGDIR) \
		  $(DEFAULT_VARDIR) \
		  $(DEFAULT_VARDIR)/seq \
		  $(DEFAULT_LOCKDIR) \
		  $(DEFAULT_SPOOLDIR) \
		  $(DEFAULT_SPOOLDIR)/outrfc \
		  $(DEFAULT_SPOOLDIR)/outrfc/mail \
		  $(DEFAULT_SPOOLDIR)/outrfc/news \
		  $(DEFAULT_SPOOLDIR)/outpkt \
		  $(DEFAULT_SPOOLDIR)/outpkt/mail \
		  $(DEFAULT_SPOOLDIR)/outpkt/news \
		  $(DEFAULT_SPOOLDIR)/toss \
		  $(DEFAULT_SPOOLDIR)/toss/toss \
		  $(DEFAULT_SPOOLDIR)/toss/route \
		  $(DEFAULT_SPOOLDIR)/toss/pack \
		  $(DEFAULT_SPOOLDIR)/toss/bad \
		  $(DEFAULT_BTBASEDIR) \
		  $(DEFAULT_INBOUND) \
		  $(DEFAULT_PINBOUND) \
		  $(DEFAULT_UUINBOUND) \
		  $(DEFAULT_FTPINBOUND)



all clean veryclean check depend install::
	for d in $(SUBDIRS); do \
	  if [ -f $$d/Makefile ]; then $(MAKE) -C $$d $@ || exit 1; fi; \
	done

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core

install-dirs:
	for d in $(INSTALLDIRS); do if [ ! -d $$d ]; then \
	    echo "Creating $$d ..."; $(INSTALL_DIR) $(PREFIX)$$d; \
	fi; done

install::
	cp ANNOUNCE $(PREFIX)$(HTMLDIR)

tags:
	etags *.[hcy] *.pl *.make Makefile */Makefile doc/*.texi doc/*.html \
	  */*/*.[hcy] */*/*.pl */*/Makefile
