#!/bin/sh
#
# $Id: runnews.sh,v 4.2 1998/02/14 17:13:58 mj Exp $
#
# Toss gateway output news
#
# Usage: runnews
#

PRG=<LIBDIR>
LOCK=runnews

# Output to "log-out" log file
FIDOGATE_LOGFILE="%G/log-news"
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
