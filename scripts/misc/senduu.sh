#!/bin/sh
#
# $Id: senduu.sh,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Send arcmail archives via UUENCODEd mail
#

LIBDIR="<LIBDIR>"
OUTBOUND="<OUTBOUND>"

# Bounced mail to /dev/null (nobody alias)
ENVELOPE="-fnobody@dfv.rwth-aachen.de"

# MTA
MAILER="/usr/lib/sendmail"


# send function
senduumail () {
	# create bsy file
	$LIBDIR/ftnbsy -l $node
	st=$?
	if [ $st -ne 0 ]; then
		exit 2
	fi

	# send files
	for f in $arc.*; do if [ -f $f ]; then
		# mail file
		uuencode $f `basename $f` | $MAILER $ENVELOPE $user
		st=$?
		if [ $st -ne 0 ]; then
			echo "ERROR: sending mail failed"
			$LIBDIR/ftnbsy -u $node
			exit 1
		fi

#		# truncate file
#		: >$f
		# remove file
		rm -f $f
	fi; done

	# remove FLO file
	rm -f $flo

	# delete bsy file
	$LIBDIR/ftnbsy -u $node
}


# project 2:2433/225
node="2:2433/225"
user="fido@project.gun.de"
flo="$OUTBOUND/out/098100e1.hlo"
arc="$OUTBOUND/out/0013ff8d"

senduumail

# project 242:4000/0
node="242:4000/0"
user="fido@project.gun.de"
flo="$OUTBOUND/out.0f2/0fa00000.hlo"
arc="$OUTBOUND/out.0f2/f9f4006e"

senduumail

# project 242:4000/1
node="242:4000/1"
user="fido@project.gun.de"
flo="$OUTBOUND/out.0f2/0fa00001.hlo"
arc="$OUTBOUND/out.0f2/f9f4006d"

senduumail
