#!/bin/sh
#
# $Id: runffx.sh,v 4.1 1998/11/08 18:28:00 mj Exp $
#
# Run ffx programs
#
# Usage: rungate
#

LOCK=runffx

# Output to "log-in" log file
FIDOGATE_LOGFILE="%G/log-ffx"
export FIDOGATE_LOGFILE

# Lock it
<LIBDIR>/ftnlock -l $LOCK $$
st=$?
if [ $st -ne 0 ]; then
	exit 2
fi


# process news for ffx
<NEWSETCDIR>/send-ffx orodruin

# batch ffx
# CUSTOMIZE! ----vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
<LIBDIR>/ftnpack -f 242:1000/5 -I %B/out.0f2/orodruin

# process inbound ffx files
<LIBDIR>/ffxqt


# Unlock it
<LIBDIR>/ftnlock -u $LOCK

exit 0
