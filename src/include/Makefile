#:ts=8
#
#    $Id: Makefile,v 4.1 1996/05/03 19:18:45 mj Exp $
#
#    Makefile for FIDOGATE src/include/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


all:		paths.mak

paths.mak:	$(TOPDIR)/config.make
	@echo '/*'                                          > paths.mak
	@echo ' * Automatically generated --- do not edit!' >>paths.mak
	@echo ' */'                                         >>paths.mak
	@echo '#define BINDIR   "$(BINDIR)"'                >>paths.mak
	@echo '#define LIBDIR   "$(LIBDIR)"'                >>paths.mak
	@echo '#define SPOOLDIR "$(SPOOLDIR)"'              >>paths.mak
	@echo '#define LOGDIR   "$(LOGDIR)"'                >>paths.mak
	@echo ' '                                           >>paths.mak
	@echo '#define OUTBOUND "$(OUTBOUND)"'              >>paths.mak
	@echo '#define INBOUND  "$(INBOUND)"'               >>paths.mak
	@echo '#define PINBOUND "$(PINBOUND)"'              >>paths.mak
	diff paths.mak paths.h >/dev/null || cp paths.mak paths.h

install:


clean:
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:

depend:
