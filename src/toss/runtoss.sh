#!/bin/sh
#
# $Id: runtoss.sh,v 4.0 1996/04/17 18:17:42 mj Exp $
#
# Wrapper for ftntoss, ftnroute, ftnpack doing the toss process
#
# Usage: runtoss name
#

PRG=<LIBDIR>
SPOOL=<SPOOLDIR>
OUTBOUND=<OUTBOUND>
INBOUND=<INBOUND>
PINBOUND=<PINBOUND>
UUINBOUND=<UUINBOUND>


MAXFILES=30
MAXMSG=400
#MAXARC=3000
MAXARC=2048

MINFREE=15000				# Need at least 15 MB free!



if [ $# -ne 1 ]; then
	echo "usage: runtoss name"
	exit 1
fi

NAME=$1

#
# Check free disk space
#
free=`$PRG/rundf $SPOOL`
if [ $free = "ERROR" ]; then
	echo "ERROR: running rundf $SPOOL"
	$PRG/ftnlog -p runtoss "ERROR: running rundf $SPOOL"
	exit 1
fi
if [ $free -lt $MINFREE ]; then
	echo "ERROR: $SPOOL disk space low, $free K"
	$PRG/ftnlog -p runtoss "ERROR: $SPOOL disk space low, $free K"
	exit 1
fi


#
# Set input and grade depending on NAME
#
# Set FADIR to "" if you don't want to process file attaches
#
case X$NAME in
	Xnormal)
		INPUT=
		FADIR=
		GRADE=
		FLAGS=-s
		;;
	Xout)
		INPUT=-I$SPOOL/out
		FADIR=
		GRADE=-gn
		FLAGS="-n -t"
		;;
	Xpin)
		INPUT=-I$PINBOUND
		FADIR=-F$PINBOUND
		GRADE=-gp
		FLAGS=-s
		;;
	Xin)
		INPUT=-I$INBOUND
		FADIR=-F$INBOUND
		GRADE=-gi
		FLAGS=-s
		;;
	Xuuin)
		INPUT=-I$UUINBOUND
		FADIR=
		GRADE=-gu
		FLAGS=-s
		;;
	*)
		echo "runtoss: unknown $NAME"
		exit 1
		;;
esac


#
# ftntoss/route/pack options
#
OPT=


#
# Processing ftntoss/ftnroute/ftnpack
#
flag="cont"

while [ "$flag" = "cont" ]; do

	#
	# Check free disk space again - outbound
	#
	free=`$PRG/rundf $OUTBOUND`
	if [ $free = "ERROR" ]; then
		echo "ERROR: running rundf $OUTBOUND"
		$PRG/ftnlog -p runtoss "ERROR: running rundf $OUTBOUND"
		exit 1
	fi
	if [ $free -lt $MINFREE ]; then
		echo "ERROR: $OUTBOUND disk space low, $free K"
		$PRG/ftnlog -p runtoss "ERROR: $OUTBOUND disk space low, $free K"
		exit 1
	fi

	$PRG/ftntoss $OPT -M$MAXFILES -x$MAXMSG $INPUT $GRADE $FLAGS
	st=$?
	if   [ $st -eq 0 ]; then
		flag="exit";
	elif [ $st -eq 3 ]; then
		flag="cont";
	else
		echo "ERROR: ftntoss exit $st"
		exit 1
	fi
	
	$PRG/ftnroute $OPT -M$MAXFILES $GRADE
	st=$?
	if   [ $st -ne 0 ]; then
		echo "ERROR: ftnroute exit $st"
		exit 1
	fi
	
	$PRG/ftnpack $OPT -m$MAXARC $FADIR $GRADE
	st=$?
	if   [ $st -ne 0 ]; then
		echo "ERROR: ftnpack exit $st"
		exit 1
	fi

done


exit 0
