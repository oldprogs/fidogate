#:ts=8
#
#    $Id: Makefile,v 4.14 1998/02/24 17:27:09 mj Exp $
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


PROGS		= ftn2ftn ftnexpire ftnpack ftnroute ftntoss\
		  rundf runin runout runmail runnews runtoss rununpack

OBJS_TOSS	= ftntoss.o dbz.o history.o
OBJS_EXPIRE	= ftnexpire.o dbz.o

LOCAL_CFLAGS	= -I.
LOCAL_LFLAGS	=


all:		$(PROGS)

ftn2ftn:	$(TOPDIR)/src/common/$(LIB)
ftnpack:	$(TOPDIR)/src/common/$(LIB)
ftnroute:	$(TOPDIR)/src/common/$(LIB)

ftntoss:	$(TOPDIR)/src/common/$(LIB) $(OBJS_TOSS)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftntoss $(OBJS_TOSS) $(LIBS)

ftnexpire:	$(TOPDIR)/src/common/$(LIB) $(OBJS_EXPIRE)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o ftnexpire $(OBJS_EXPIRE) $(LIBS)


install:
	$(INSTALL_PROG) ftn2ftn$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftntoss$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftnexpire$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftnroute$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) ftnpack$(EXE) $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) runtoss $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) rununpack $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) magic $(PREFIX)$(LIBDIR)
	$(INSTALL_PROG) runin $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) runmail $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) runnews $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) runout $(PREFIX)$(BINDIR)
	$(INSTALL_PROG) rundf $(PREFIX)$(LIBDIR)



clean veryclean:
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
