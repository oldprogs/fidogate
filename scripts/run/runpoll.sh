#!/bin/sh
#
# $Id: runpoll.sh,v 4.4 1996/11/30 15:46:12 mj Exp $
#
# Poll uplink
#

FIDOGATE=/usr/local/lib/fidogate
IFMAIL=/usr/local/lib/ifmail
NEWS=/etc/news

UPLINK=f2.n1000.z242

# -xterm: run in XTerm window
if [ "$1" = "-xterm" ]; then
  exec /usr/bin/X11/xterm -display :0 -g 80x20 -title "FIDOGATE runpoll" -e $FIDOGATE/runpoll
  exit 0
fi


# Show executed commands
set -x


# Batch ffx mail files
$FIDOGATE/ffxbatch -F Normal -w -b morannon 242:1000/1

# Batch ffx news
$NEWS/send-ffx

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
