#!/bin/sh
#
# $Id: runin.sh,v 4.4 1998/04/07 12:21:59 mj Exp $
#
# Toss inbound, protected inbound, uuencode inbound, ftp inbound
#
# Usage: runin
#

LOCK=runin

# Output to "log-in" log file
FIDOGATE_LOGFILE="%G/log-in"
export FIDOGATE_LOGFILE

# Lock it
<LIBDIR>/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi

if [ -d <INBOUND> ]; then
    <BINDIR>/rununpack in
    <BINDIR>/runtoss   in
fi

if [ -d <PINBOUND> ]; then
    <BINDIR>/rununpack pin
    <BINDIR>/runtoss   pin
fi

if [ -d <UUINBOUND> ]; then
    <BINDIR>/rununpack uuin
    <BINDIR>/runtoss   uuin
fi

if [ -d <FTPINBOUND> ]; then
    <BINDIR>/rununpack ftpin
    <BINDIR>/runtoss   ftpin
fi

# Unlock it
<LIBDIR>/ftnlock -u $LOCK

exit 0
