#!/bin/sh
#
# $Id: run1.sh,v 4.3 1997/11/16 15:53:24 mj Exp $
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

### toss gateway output ###
$PRG/runout


### Unlock it ###
$PRG/ftnlock -u $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 1
fi

exit 0
