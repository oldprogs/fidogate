#:ts=8
#
#    $Id: Makefile,v 1.2 1998/05/01 15:16:43 mj Exp $
#
#    Makefile for FIDOGATE src/charset/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


PROGS		= charsetc

OBJS_CHARSETC	= charsetc.o new.o

LOCAL_CFLAGS	=
LOCAL_LFLAGS	=

TESTS		=


all:		$(PROGS)

charsetc:	$(TOPDIR)/src/common/$(LIB) $(OBJS_CHARSETC)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o charsetc $(OBJS_CHARSETC) $(LIBS)


testcharset:	testcharset.o $(TOPDIR)/src/common/$(LIB)
	$(CC) $(LFLAGS) -o testcharset testcharset.o $(LIBS)
testcharset.o:	new.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -DTEST -c new.c -o testcharset.o



install:


clean veryclean:
	rm -f $(PROGS)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Test programs
#
check:		$(TESTS)



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
