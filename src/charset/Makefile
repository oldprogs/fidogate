#:ts=8
#
#    $Id: Makefile,v 1.3 1998/05/03 12:46:29 mj Exp $
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


LIBA		= $(TOPDIR)/src/common/$(LIB)

OBJS		= charset.o

PROGS		= charsetc

OBJS_CHARSETC	= charsetc.o charset.o

LOCAL_CFLAGS	=
LOCAL_LFLAGS	=

LIBOBJS		= $(OBJS:%=$(LIBA)(%))

TESTS		= testcharset

CHARSETMAP	= charset.bin
CHARSETSRC	= charset.map


all:		$(LIBA) $(PROGS) $(CHARSETMAP)

$(LIBA):	$(LIBOBJS)
	$(RANLIB) $(LIBA)

charsetc:	$(LIBA) $(OBJS_CHARSETC)
	$(CC) $(LFLAGS) $(LOCAL_LFLAGS) -o charsetc $(OBJS_CHARSETC) $(LIBS)


$(CHARSETMAP):	$(CHARSETSRC)
	./charsetc -v $(CHARSETSRC) $(CHARSETMAP)


install:
	$(INSTALL_DATA) $(CHARSETMAP) $(PREFIX)$(LIBDIR)


clean veryclean:
	rm -f $(PROGS) $(TESTS) $(CHARSETMAP)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp


#
# Test programs
#
check:		$(TESTS)

testcharset:	testcharset.o $(TOPDIR)/src/common/$(LIB)
	$(CC) $(LFLAGS) -o testcharset testcharset.o $(LIBS)
testcharset.o:	charset.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -DTEST -c charset.c -o testcharset.o


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