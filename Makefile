#
# $Id: Makefile,v 4.16 1999/10/17 11:49:26 mj Exp $
#
# Makefile FIDOGATE TOPDIR
#

TOPDIR		= .

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

SUBDIRS		= src scripts test doc sendmail

INSTALLDIRS	= $(DEFAULT_V_CONFIGDIR) \
		  $(DEFAULT_V_LIBDIR) \
		  $(DEFAULT_V_BINDIR) \
		  $(DEFAULT_V_LOGDIR) \
		  $(DEFAULT_V_VARDIR) \
		  $(DEFAULT_V_VARDIR)/seq \
		  $(DEFAULT_V_LOCKDIR) \
		  $(DEFAULT_V_SPOOLDIR) \
		  $(DEFAULT_V_SPOOLDIR)/outrfc \
		  $(DEFAULT_V_SPOOLDIR)/outrfc/mail \
		  $(DEFAULT_V_SPOOLDIR)/outrfc/news \
		  $(DEFAULT_V_SPOOLDIR)/outpkt \
		  $(DEFAULT_V_SPOOLDIR)/outpkt/mail \
		  $(DEFAULT_V_SPOOLDIR)/outpkt/news \
		  $(DEFAULT_V_SPOOLDIR)/toss \
		  $(DEFAULT_V_SPOOLDIR)/toss/toss \
		  $(DEFAULT_V_SPOOLDIR)/toss/route \
		  $(DEFAULT_V_SPOOLDIR)/toss/pack \
		  $(DEFAULT_V_SPOOLDIR)/toss/bad \
		  $(DEFAULT_V_BTBASEDIR) \
		  $(DEFAULT_V_BTBASEDIR)/tick \
		  $(DEFAULT_V_BTBASEDIR)/ffx \
		  $(DEFAULT_V_INBOUND) \
		  $(DEFAULT_V_PINBOUND) \
		  $(DEFAULT_V_UUINBOUND) \
		  $(DEFAULT_V_FTPINBOUND) \
		  $(INFODIR) $(HTMLDIR)



all clean veryclean check test verify depend install::
	for d in $(SUBDIRS); do \
	  if [ -f $$d/Makefile ]; then $(MAKE) -C $$d $@ || exit 1; fi; \
	done

clean veryclean::
	rm -f *~ *.bak *.o tags TAGS core

install-dirs:
	for d in $(INSTALLDIRS); do if [ ! -d $$d ]; then \
	    echo "Creating $$d ..."; $(INSTALL_DIR) $(PREFIX)$$d; \
	fi; done

install-uuin:
	if [ ! -d $$d ]; then \
	    $(INSTALL_DIR) $(PREFIX)$(DEFAULT_V_UUINBOUND); \
	fi
	chgrp mail $(PREFIX)$(DEFAULT_V_UUINBOUND)
	chmod g+w  $(PREFIX)$(DEFAULT_V_UUINBOUND)

install::
	cp ANNOUNCE $(PREFIX)$(HTMLDIR)

tags:
	etags *.[hcy] *.pl *.make Makefile */Makefile doc/*.texi doc/*.html \
	  */*/*.[hcy] */*/*.pl */*/Makefile
