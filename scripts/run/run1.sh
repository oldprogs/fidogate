#!/bin/sh
#
# $Id: run1.sh,v 4.0 1996/04/17 18:17:38 mj Exp $
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
NEWS=/usr/lib/news

LOCK=runfso


#
# Lock it
#
$PRG/ftnlock -l $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


#
# 1st pass: process inbound ffx files
#
# unbatch mail, process mail jobs (grade `f')
$PRG/ffxqt -gf
$PRG/ffxqt -gf

# process news jobs (grade `n')
$PRG/ffxqt -gn

#
# 2nd pass: process news batches for gateway
#
$NEWS/send-fidogate

#
# 3rd pass: toss gateway output
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
