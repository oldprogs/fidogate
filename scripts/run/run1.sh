#!/bin/sh
#
# $Id: run1.sh,v 4.2 1997/06/21 21:16:43 mj Exp $
#
# Example script for FIDOGATE gateway out processing
#

PRG=<LIBDIR>
LIB=<LIBDIR>
NEWS=<NEWSETCDIR>

LOCK=run1


### Lock it ###
$PRG/ftnlock -l $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


### process news for gateway ###
$NEWS/send-fidogate

### process news for ffx ###
$NEWS/send-ffx

### batch ffx ###
#$PRG/ffxbatch -F Hold -w -b orodruin 242:1000/5
$PRG/ftnpack -f 242:1000/5 -I %O/out.0f2/orodruin

### toss gateway output ###
$PRG/runout


### Unlock it ###
$PRG/ftnlock -u $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 1
fi

exit 0
