#:ts=8
#
# $Id: rules.make,v 4.0 1996/04/17 18:17:32 mj Exp $
#
# Common rules for all FIDOGATE Makefiles
#

.SUFFIXES: .pl .sh

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

#$(LIB)(%.o):	%.o
#	$(AR) r $(LIB) $<
