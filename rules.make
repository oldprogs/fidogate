# -*- Makefile -*-
#
# $Id: rules.make,v 4.3 1997/11/16 15:53:21 mj Exp $
#
# Common rules for all FIDOGATE Makefiles
#

.SUFFIXES: .pl .sh .mc .cf

%.o:		%.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -c $<

%:		%.o
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o $* $*.o $(LIBS)

%:		%.pl
	$(PERL) $(TOPDIR)/subst.pl -c$(TOPDIR)/config.make -p$(PERL) $< >$*
	chmod +x $*

%:		%.sh
	$(PERL) $(TOPDIR)/subst.pl -c$(TOPDIR)/config.make $< >$*
	chmod +x $*

%.cf:		%.mc
	$(M4) $(M4OPTIONS) $< >$*.cf

#$(LIB)(%.o):	%.o
#	$(AR) r $(LIB) $<
