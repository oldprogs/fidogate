#!/bin/sh
#
# $Id: runin.sh,v 4.2 1996/11/30 15:46:13 mj Exp $
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
# Output to "log-in" log file
#
LOGFILE="$LOGDIR/log-in"
export LOGFILE

#
# Lock it
#
$PRG/ftnlock -l runin $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

$PRG/rununpack in
$PRG/runtoss   in

$PRG/rununpack pin
$PRG/runtoss   pin

$PRG/rununpack uuin
$PRG/runtoss   uuin

#
# Unlock it
#
$PRG/ftnlock -u runin

exit 0
