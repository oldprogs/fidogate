#!/bin/sh
#
# $Id: runout.sh,v 4.1 1996/11/25 19:52:00 mj Exp $
#
# Run protected and normal inbound tossing (rununpack, runtoss)
#
# Usage: runin
#

PRG=<LIBDIR>
LOGDIR=`$PRG/ftnconfig =logdir`

if [ -z "$LOGDIR" ]; then
  echo "runtoss: parameter logdir missing"
  exit 1
fi


#
# Output to "log-out" log file
#
LOGFILE="$LOGDIR/log-out"
export LOGFILE

#
# Lock it
#
$PRG/ftnlock -l runout
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

$PRG/runtoss out

#
# Unlock it
#
$PRG/ftnlock -u runout

exit 0
