#!/bin/sh
#
# $Id: runout.sh,v 4.0 1996/04/17 18:17:42 mj Exp $
#
# Run protected and normal inbound tossing (rununpack, runtoss)
#
# Usage: runin
#

PRG=<LIBDIR>
LIB=<LIBDIR>

#
# Output to "log-out" log file
#
LOGFILE="<LOGDIR>/log-out"
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
