#!/bin/sh
#
# $Id: runmail.sh,v 4.3 1998/02/14 17:13:58 mj Exp $
#
# Toss gateway output mail
#
# Usage: runmail
#

PRG=<LIBDIR>
LOCK=runmail

# Output to "log-out" log file
FIDOGATE_LOGFILE="%G/log-mail"
export FIDOGATE_LOGFILE

# Lock it
$PRG/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

$PRG/runtoss outpkt/mail

# Unlock it
$PRG/ftnlock -u $LOCK

exit 0
