#!/bin/sh
#
# $Id: run2.sh,v 4.3 1997/06/21 21:16:43 mj Exp $
#
# Example script for FIDOGATE inbound processing
#

PRG=<LIBDIR>
LIB=<LIBDIR>
NEWS=<NEWSETCDIR>

LOCK=run2


### Lock it ###
$PRG/ftnlock -l $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


### toss inbound ###
$PRG/runin

### process inbound ffx files ###
# unbatch mail, process mail jobs (grade `f')
$PRG/ffxqt -gf
$PRG/ffxqt -gf
# process news jobs (grade `n')
$PRG/ffxqt -gn

### process packets for Internet gateway ###
$PRG/ftnin -x %L/ftninpost

### process packets for FTN gateway ###
$PRG/ftn2ftn -A 2:2/242 -B 242:242/2

### process tic files ###
$PRG/ftntick


### Unlock it ###
$PRG/ftnlock -u $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 1
fi

exit 0
