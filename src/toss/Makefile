#:ts=8
#
#    $Id: Makefile,v 4.3 1996/05/05 12:26:57 mj Exp $
#
#    Makefile for FIDOGATE src/toss/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= ftn2ftn ftnaf ftnexpire ftnpack ftnroute ftntoss\
		  rundf runin runout runtoss rununpack
# + ftnbounce

OBJS_TOSS	= ftntoss.o dbz.o history.o
OBJS_EXPIRE	= ftnexpire.o dbz.o

LOCAL_CFLAGS	= -I.
LOCAL_LFLAGS	=


all:		$(PROGS)

ftn2ftn:	$(TOPDIR)/src/common/$(LIB)
ftnaf:		$(TOPDIR)/src/common/$(LIB)
ftnpack:	$(TOPDIR)/src/common/$(LIB)
ftnroute:	$(TOPDIR)/src/common/$(LIB)

ftntoss:	$(TOPDIR)/src/common/$(LIB) $(OBJS_TOSS)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftntoss $(OBJS_TOSS) $(LIBS)

ftnexpire:	$(TOPDIR)/src/common/$(LIB) $(OBJS_EXPIRE)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftnexpire $(OBJS_EXPIRE) $(LIBS)


install:
	$(INSTALL_PROG)   ftn2ftn     $(LIBDIR)
	$(INSTALL_PROG)   ftntoss     $(LIBDIR)
	$(INSTALL_PROG)   ftnexpire   $(LIBDIR)
	$(INSTALL_PROG)   ftnroute    $(LIBDIR)
	$(INSTALL_PROG)   ftnpack     $(LIBDIR)
	$(INSTALL_PROG)   runtoss     $(LIBDIR)
	$(INSTALL_PROG)   rununpack   $(LIBDIR)
	$(INSTALL_PROG)   runin       $(LIBDIR)
	$(INSTALL_PROG)   runout      $(LIBDIR)
	$(INSTALL_PROG)   rundf       $(LIBDIR)
	$(INSTALL_SETUID) ftnaf       $(LIBDIR)



clean:
	rm -f $(PROGS)
	rm -f testhistory
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Test programs
#
check:		testhistory

testhistory:	testhistory.o dbz.o
	$(CC) $(LFLAGS) -o testhistory testhistory.o dbz.o $(LIBS)
testhistory.o:	history.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -DTEST -c history.c -o testhistory.o


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
