#!/bin/sh
#
# $Id: run1.sh,v 4.1 1997/04/18 14:12:27 mj Exp $
#
# Run
#     ffxqt
#     send-fidogate
#     runout
#
# Usage: runfso
#

PRG=<LIBDIR>
LIB=<LIBDIR>
NEWS=<NEWSETCDIR>

LOCK=run1


#
# Lock it
#
$PRG/ftnlock -l $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


#
# process news batches for gateway
#
$NEWS/send-fidogate

#
# toss gateway output
#
$PRG/runout


#
# Unlock it
#
$PRG/ftnlock -u $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 1
fi

exit 0
