#!/bin/sh
#
# $Id: runout.sh,v 4.4 1998/02/14 17:13:59 mj Exp $
#
# Toss misc output (ftnafpkt, ftnoutpkt)
#
# Usage: runout
#

PRG=<LIBDIR>
LOCK=runout

# Output to "log-out" log file
FIDOGATE_LOGFILE="%G/log-out"
export FIDOGATE_LOGFILE

# Lock it
$PRG/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

$PRG/runtoss outpkt

# Unlock it
$PRG/ftnlock -u $LOCK

exit 0
