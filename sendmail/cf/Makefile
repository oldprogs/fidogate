#:ts=8
#
#    $Id: Makefile,v 4.5 1997/10/05 13:43:32 mj Exp $
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


CF		= orodruin.cf orodruin-smtp.cf orodruin-check.cf\
		  morannon.cf morannon-ffx.cf morannon-check.cf\
		  point.cf


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
