#:ts=8
#
# $Id: rules.make,v 4.1 1996/09/22 12:26:44 mj Exp $
#
# Common rules for all FIDOGATE Makefiles
#

.SUFFIXES: .pl .sh .mc .cf

%.o:		%.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -c $<

%:		%.o
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o $* $*.o $(LIBS)

%:		%.pl
	$(PERL) $(TOPDIR)/mksubst.pl -c$(TOPDIR)/config.make -p$(PERL) $< >$*
	chmod +x $*

%:		%.sh
	$(PERL) $(TOPDIR)/mksubst.pl -c$(TOPDIR)/config.make $< >$*
	chmod +x $*

%.cf:		%.mc
	$(M4) $< >$*.cf

#$(LIB)(%.o):	%.o
#	$(AR) r $(LIB) $<
