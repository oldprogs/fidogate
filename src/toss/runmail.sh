#!/bin/sh
#
# $Id: runmail.sh,v 4.1 1997/11/16 15:53:27 mj Exp $
#
# Run protected and normal inbound tossing (rununpack, runtoss)
#
# Usage: runin
#

PRG=<LIBDIR>
LOGDIR=`$PRG/ftnconfig =logdir`

LOCK=runmail

if [ -z "$LOGDIR" ]; then
  echo "runtoss: parameter logdir missing"
  exit 1
fi


#
# Output to "log-out" log file
#
LOGFILE="$LOGDIR/log-mail"
export LOGFILE

#
# Lock it
#
$PRG/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

$PRG/runtoss outpkt/mail

#
# Unlock it
#
$PRG/ftnlock -u $LOCK

exit 0
