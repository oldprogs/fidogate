#!/bin/sh
#
# $Id: runpoll.sh,v 4.5 1997/04/18 14:12:28 mj Exp $
#
# Poll uplink
#

FIDOGATE=<LIBDIR>
IFMAIL=<IFMAILDIR>
NEWS=<NEWSETCDIR>

UPLINK=f2.n1000.z242

# -xterm: run in XTerm window
if [ "$1" = "-xterm" ]; then
  exec /usr/bin/X11/xterm -display :0 -g 80x20 -title "FIDOGATE runpoll" -e $FIDOGATE/runpoll
  exit 0
fi


# Show executed commands
set -x


# Batch ffx news
$NEWS/send-ffx

# Batch ffx mail
#$FIDOGATE/ffxbatch -F Normal -w -b morannon 242:1000/1
$FIDOGATE/ftnpack -f 242:1000/1 -I %O/out.0f2/morannon

# Gateway
$NEWS/send-fidogate

# Tosser w/file attachments
$FIDOGATE/runtoss outf
# Tosser w/o file attachments
#$FIDOGATE/runtoss out

# Poll
$IFMAIL/ifcico $UPLINK

# Unbatch and process ffx files
$FIDOGATE/ffxrun

# Process tic files
$FIDOGATE/ftntick

# Tosser
$FIDOGATE/rununpack pin
$FIDOGATE/runtoss   pin

# Gateway
$FIDOGATE/ftnin -x %L/ftninpost

# Process mail queue
/usr/sbin/sendmail -q

# Tosser expire
$FIDOGATE/ftnexpire
