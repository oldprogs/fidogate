#:ts=8
#
#    $Id: Makefile,v 4.2 1996/09/22 20:12:36 mj Exp $
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


CF		= orodruin.cf orodruin-smtp.cf\
		  morannon.cf morannon-ffx.cf\
		  point.cf


all:		$(CF)


install:

install-orodruin:
	install -g root -o root -m 644 orodruin.cf          /etc/sendmail.cf
	install -g root -o root -m 644 mailertable-orodruin /etc/mail/mailertable
	install -g root -o root -m 644 rewrite-orodruin     /etc/mail/rewrite
	install -g root -o root -m 755 mkmaildb             /etc/mail
	/etc/mail/mkmaildb
	killall sendmail
	/usr/sbin/sendmail -bd -q15m

install-morannon:
	install -g root -o root -m 644 morannon.cf          /etc/sendmail.cf
	install -g root -o root -m 644 mailertable-morannon /etc/mail/mailertable
	install -g root -o root -m 644 rewrite-morannon     /etc/mail/rewrite
	install -g root -o root -m 755 mkmaildb             /etc/mail
	/etc/mail/mkmaildb
	killall sendmail
	/usr/sbin/sendmail -bd -q15m


clean:
	rm -f $(CF)
	rm -f *~ *.bak *.o tags TAGS core paths.tmp

check:


#
# Dependencies
#
depend:

depend-all:
