#!/bin/sh
#
# $Id: runin.sh,v 4.0 1996/04/17 18:17:42 mj Exp $
#
# Run protected and normal inbound tossing (rununpack, runtoss)
#
# Usage: runin
#

PRG=<LIBDIR>
LIB=<LIBDIR>

#
# Output to "log-in" log file
#
LOGFILE="<LOGDIR>/log-in"
export LOGFILE

#
# Lock it
#
$PRG/ftnlock -l runin
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
