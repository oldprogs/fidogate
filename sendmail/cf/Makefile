#:ts=8
#
#    $Id: Makefile,v 4.8 1999/07/18 15:23:49 mj Exp $
#
#    Makefile for FIDOGATE sendmail/cf/
#

#
# Normally it should not be necessary to modify this file, see
# ../config.make and ../rules.make
#

TOPDIR		= ../..

include $(TOPDIR)/config.make
include $(TOPDIR)/rules.make

M4OPTIONS	= -DconfFIDOGATE_LIBDIR=$(LIBDIR)
#M4OPTIONS	= -DconfFIDOGATE_LIBDIR=$(LIBDIR) \
#		  -DconfFIDOGATE_OUTDIR=%S/outpkt/mail


CF		= redhat6-orodruin.cf
#CF		= orodruin.cf orodruin-smtp.cf orodruin-check.cf\
#		  morannon.cf morannon-ffx.cf morannon-check.cf\
#		  point.cf


all:		$(CF)


install:


clean:
	rm -f $(CF)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:


#
# Dependencies
#
$(CF):		../mailer/ftn.m4 ../mailer/ffx.m4

depend:

depend-all:
