#:ts=8
#
#    $Id: Makefile,v 1.1 1998/02/07 18:35:19 mj Exp $
#
#    Makefile for FIDOGATE src/areafix/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= ftnaf ftnafmail

OBJS_AF		= areafix.o

LOCAL_CFLAGS	=
LOCAL_LFLAGS	=


all:		$(PROGS)

ftnafmail:	$(TOPDIR)/src/common/$(LIB)

ftnaf:		$(TOPDIR)/src/common/$(LIB) $(OBJS_AF)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftnaf ftnaf.o $(OBJS_AF) $(LIBS)


install:
	$(INSTALL_PROG) ftnaf$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_SETUID) ftnafmail$(EXE) $(PREFIX)$(LIBDIR)



clean veryclean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Test programs
#
check:


#
# Dependencies
#
depend:
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -MM *.c >.depend

depend-all:
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -M *.c >.depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
