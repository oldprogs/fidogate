#!/bin/sh
#
# $Id: runnews.sh,v 4.1 1998/01/24 15:45:57 mj Exp $
#
# Toss gateway output news
#
# Usage: runnews
#

PRG=<LIBDIR>
LOCK=runnews

# Output to "log-out" log file
FIDOGATE_LOGFILE="$LOGDIR/log-news"
export FIDOGATE_LOGFILE

# Lock it
$PRG/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

$PRG/runtoss outpkt/news

# Unlock it
$PRG/ftnlock -u $LOCK

exit 0
