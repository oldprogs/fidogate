#!/bin/sh
#
# $Id: runpoll-orodruin.sh,v 4.1 1999/01/02 16:34:53 mj Exp $
#
# Poll uplink
#

LIBDIR=<LIBDIR>
BINDIR=<BINDIR>
IFMAIL=<IFMAILDIR>
NEWS=<NEWSETCDIR>

UPLINK=f2.n1000.z242

# -xterm: run in XTerm window
if [ "$1" = "-xterm" ]; then
  exec /usr/bin/X11/xterm -display :0 -g 80x20 -title "FIDOGATE runpoll" -e $BINDIR/runpoll
  exit 0
fi


# Show executed commands
set -x


# Batch ffx news
$NEWS/send-ffx

# Batch ffx mail
$LIBDIR/ftnpack -f 242:1000/1 -I %B/out.0f2/morannon

# Gateway
$NEWS/send-fidogate

# Tosser w/o file attachments
$BINDIR/runtoss outpkt
$BINDIR/runtoss outpkt/mail
$BINDIR/runtoss outpkt/news

# Poll
$IFMAIL/ifcico $UPLINK

# Tosser, only protected inbound
$BINDIR/rununpack pin
$BINDIR/runtoss   pin

# Gateway
$LIBDIR/ftnin -x %L/ftninpost

# Unbatch and process ffx files
$LIBDIR/ffxqt

# Process tic files
$LIBDIR/ftntick

# Process mail queue
/usr/sbin/sendmail -q

# Tosser expire
$LIBDIR/ftnexpire
