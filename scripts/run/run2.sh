#!/bin/sh
#
# $Id: run2.sh,v 4.1 1996/05/07 19:50:41 mj Exp $
#
# Run
#     runin
#     ftnin
#     ftn2ftn
#
# Usage: runii2
#

PRG=<LIBDIR>
LIB=<LIBDIR>
NEWS=/usr/lib/news

LOCK=runii2


#
# Lock it
#
$PRG/ftnlock -l $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


#
# 1st pass: toss inbound
#
$PRG/runin

#
# 2nd pass: process packets for Internet gateway
#
$PRG/ftnin -x %L/ftninpost

#
# 3rd pass: process packets for FTN gateway
#
$PRG/ftn2ftn -A 2:2/242 -B 242:242/2


#
# Unlock it
#
$PRG/ftnlock -u $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 1
fi

exit 0
