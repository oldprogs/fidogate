# -*- Makefile -*-
#
# $Id: rules.make,v 4.5 1998/02/25 09:15:19 mj Exp $
#
# Common rules for all FIDOGATE Makefiles
#

.SUFFIXES: .pl .sh .mc .cf

%.o:		%.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -c $<

%:		%.o
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o $* $*.o $(LIBS)

%:		%.pl
	$(PERL) $(TOPDIR)/subst.pl -t$(TOPDIR) -p $< >$*
	chmod +x $*

%:		%.sh
	$(PERL) $(TOPDIR)/subst.pl -t$(TOPDIR) $< >$*
	chmod +x $*

%.cf:		%.mc
	$(M4) $(M4OPTIONS) $< >$*.cf

#$(LIB)(%.o):	%.o
#	$(AR) r $(LIB) $<
