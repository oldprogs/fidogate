#:ts=8
#
#    $Id: Makefile,v 4.3 1996/06/09 10:49:01 mj Exp $
#
#    Makefile for FIDOGATE src/common/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make


OBJS		= address.o aliases.o areas.o areasbbs.o binkley.o\
		  bounce.o bufstring.o charset.o config.o crc16.o crc32.o\
		  date.o dir.o exit.o fopen.o getopt.o getopt1.o\
		  gettime.o hosts.o kludge.o lock.o log.o mail.o\
		  message.o misc.o msgid.o node.o outpkt.o packet.o passwd.o\
		  parsedate.o read.o rfcaddr.o rfcheader.o routing.o\
		  sequencer.o textlist.o version.o wildmat.o\
		  xalloc.o xstrtok.o mime.o tick.o parsenode.o flo.o

LIBOBJS		= $(OBJS:%=$(LIB)(%))


all:		$(LIB)


$(LIB):		$(LIBOBJS)
	$(RANLIB) $(LIB)

parsedate.c:	parsedate.y
	@echo "expect 6 shift/reduce conflicts"
	$(YACC) parsedate.y
	mv y.tab.c parsedate.c

install:

clean:
	rm -f $(PROGS) $(LIB)
	rm -f testparse
	rm -f *~ *.bak *.o tags TAGS core


#
# Test programs
#
check:		testparse testmatch testflo

testparse:	testparse.o
	$(CC) $(LFLAGS) -o testparse testparse.o $(LIBS)
testparse.o:	parsenode.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -DTEST -c parsenode.c -o testparse.o

testmatch:	testmatch.o
	$(CC) $(LFLAGS) -o testmatch testmatch.o $(LIBS)
testmatch.o:	wildmat.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -DTEST -c wildmat.c -o testmatch.o

testflo:	testflo.o
	$(CC) $(LFLAGS) -o testflo testflo.o $(LIBS)
testflo.o:	flo.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) -DTEST -c flo.c -o testflo.o


#
# Dependencies
#
depend:
	$(CC) $(CFLAGS) -MM *.c >.depend

depend-all:
	$(CC) $(CFLAGS) -M *.c >.depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
