#!/bin/sh
#
# $Id: runffx.sh,v 4.1 1997/11/16 15:53:25 mj Exp $
#
# Example script for FIDOGATE gateway out processing
#

PRG=<LIBDIR>
LIB=<LIBDIR>
NEWS=<NEWSETCDIR>

LOCK=runffx


### Lock it ###
$PRG/ftnlock -l $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


### process news for ffx ###
$NEWS/send-ffx

### batch ffx ###
#$PRG/ffxbatch -F Hold -w -b orodruin 242:1000/5
$PRG/ftnpack -f 242:1000/5 -I %O/out.0f2/orodruin

### process inbound ffx files ###
# unbatch mail, process mail jobs (grade `f')
$PRG/ffxqt -gf
$PRG/ffxqt -gf
# process news jobs (grade `n')
$PRG/ffxqt -gn


### Unlock it ###
$PRG/ftnlock -u $LOCK
st=$?
if [ $st -ne 0 ]; then
	exit 1
fi

exit 0
