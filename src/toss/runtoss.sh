#!/bin/sh
#
# $Id: runtoss.sh,v 4.7 1997/10/05 13:43:36 mj Exp $
#
# Wrapper for ftntoss, ftnroute, ftnpack doing the toss process
#
# Usage: runtoss name
#    or  runtoss /path/dir
#

PRG=<LIBDIR>
SPOOL=`$PRG/ftnconfig =spooldir`
OUTBOUND=`$PRG/ftnconfig =outbound`
INBOUND=`$PRG/ftnconfig =inbound`
PINBOUND=`$PRG/ftnconfig =pinbound`
UUINBOUND=`$PRG/ftnconfig =uuinbound`

if [ -z "$SPOOL" ]; then
  echo "runtoss: parameter spooldir missing"
  exit 1
fi
if [ -z "$OUTBOUND" ]; then
  echo "runtoss: parameter outbound missing"
  exit 1
fi
if [ -z "$INBOUND" ]; then
  echo "runtoss: parameter inbound missing"
  exit 1
fi
if [ -z "$PINBOUND" ]; then
  echo "runtoss: parameter pinbound missing"
  exit 1
fi
if [ -z "$UUINBOUND" ]; then
  echo "runtoss: parameter uuinbound missing"
  exit 1
fi


# Get MinDiskFree from config.main
MINFREE=`$PRG/ftnconfig MinDiskFree`
[ -z "$MINFREE" ] && MINFREE=10000		# Default


#
# Usage
#
if [ $# -eq 0 ]; then
  echo "usage: runtoss name [args ...]"
  exit 1
fi

NAME="$1"
shift
ARGS="$*"

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
	   FLAGS="-n -t -p"
	   ;;
  Xoutf)
	   INPUT=-I$SPOOL/out
	   FADIR=-F/
	   GRADE=-gn
	   FLAGS="-n -t -p"
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
  Xtest)
	   INPUT=
	   FADIR=
	   GRADE=
	   FLAGS=
	   OUTBOUND=./bt
	   SPOOL=.
	   LIBDIR=../cf
	   PRG=../../src/toss
	   ARGS="-L$PRG -S$SPOOL $ARGS"
	   ;;
  X/*)
	   INPUT=-I$NAME
	   FADIR=-F$NAME
	   GRADE=
	   FLAGS=-s
	   ;;
  *)
           echo "runtoss: unknown $NAME"
           exit 1
           ;;
esac


#
# Check free disk space in SPOOL
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

	# Toss
	$PRG/ftntoss -x $INPUT $GRADE $FLAGS $ARGS
	st=$?
	if   [ $st -eq 0 ]; then
	  flag="exit";
	elif [ $st -eq 2 ]; then
	  # busy, MSGID history or lock file
	  exit 0
	elif [ $st -eq 3 ]; then
	  flag="cont";
	else
	  echo "ERROR: ftntoss exit $st"
	  exit 1
	fi

	# Route
	$PRG/ftnroute $GRADE $ARGS
	st=$?
	if   [ $st -ne 0 ]; then
	  echo "ERROR: ftnroute exit $st"
	  exit 1
	fi

	# Pack
	$PRG/ftnpack $FADIR $GRADE $ARGS
	st=$?
	if   [ $st -ne 0 ]; then
	  echo "ERROR: ftnpack exit $st"
	  exit 1
	fi

done


exit 0
