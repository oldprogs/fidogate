#!/bin/sh
#
# $Id: runin.sh,v 4.3 1998/01/24 15:45:56 mj Exp $
#
# Toss inbound, protected inbound, uuencode inbound, ftp inbound
#
# Usage: runin
#

PRG=<LIBDIR>
LOCK=runin

# Output to "log-in" log file
FIDOGATE_LOGFILE="%G/log-in"
export FIDOGATE_LOGFILE

# Lock it
$PRG/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

if [ -d <INBOUND> ]; then
    $PRG/rununpack in
    $PRG/runtoss   in
fi

if [ -d <PINBOUND> ]; then
    $PRG/rununpack pin
    $PRG/runtoss   pin
fi

if [ -d <UUINBOUND> ]; then
    $PRG/rununpack uuin
    $PRG/runtoss   uuin
fi

if [ -d <FTPINBOUND> ]; then
    $PRG/rununpack ftpin
    $PRG/runtoss   ftpin
fi

# Unlock it
$PRG/ftnlock -u $LOCK

exit 0
