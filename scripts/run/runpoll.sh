#!/bin/sh
#
# $Id: runpoll.sh,v 4.10 1998/01/24 15:45:52 mj Exp $
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
$FIDOGATE/ftnpack -f 242:1000/1 -I %O/out.0f2/morannon

# Gateway
$NEWS/send-fidogate

# Tosser w/o file attachments
$FIDOGATE/runtoss outpkt/mail
$FIDOGATE/runtoss outpkt/news

# Poll
$IFMAIL/ifcico $UPLINK

# Tosser, only protected inbound
$FIDOGATE/rununpack pin
$FIDOGATE/runtoss   pin

# Gateway
$FIDOGATE/ftnin -x %L/ftninpost

# Unbatch and process ffx files
$FIDOGATE/ffxqt

# Process tic files
$FIDOGATE/ftntick

# Process mail queue
/usr/sbin/sendmail -q

# Tosser expire
$FIDOGATE/ftnexpire
