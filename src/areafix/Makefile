#:ts=8
#
#    $Id: Makefile,v 1.2 1998/02/14 17:13:56 mj Exp $
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


PROGS		= ftnaf ftnafmail ftnafpkt

OBJS_AF		= ftnaf.o areafix.o
OBJS_AFPKT	= ftnafpkt.o areafix.o

LOCAL_CFLAGS	=
LOCAL_LFLAGS	=

TESTS		= testpkttmpl


all:		$(PROGS)

ftnafmail:	$(TOPDIR)/src/common/$(LIB)

ftnaf:		$(TOPDIR)/src/common/$(LIB) $(OBJS_AF)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftnaf $(OBJS_AF) $(LIBS)

ftnafpkt:	$(TOPDIR)/src/common/$(LIB) $(OBJS_AFPKT)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftnafpkt $(OBJS_AFPKT) $(LIBS)


install:
	$(INSTALL_PROG) ftnaf$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftnafpkt$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_SETUID) ftnafmail$(EXE) $(PREFIX)$(LIBDIR)



clean veryclean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Test programs
#
check:		$(TESTS)

testpkttmpl:	pkttmpl.o $(TOPDIR)/src/common/$(LIB)
	$(CC) $(LFLAGS) -o testpkttmpl pkttmpl.o $(LIBS)


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