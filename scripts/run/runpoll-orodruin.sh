#!/bin/sh
#
# $Id: runpoll-orodruin.sh,v 4.4 2000/04/19 17:07:28 mj Exp $
#
# orodruin.fido.de's poll script using ifcico
#

LIBDIR=<LIBDIR>
BINDIR=<BINDIR>
IFMAIL=<IFMAILDIR>

UPLINK=f2.n1000.z242

XTERM=/usr/X11R6/bin/xterm

# -xterm: run in XTerm window
if [ "$1" = "-xterm" ]; then
  exec $XTERM -display :0 -g 100x20 -title "FIDOGATE runpoll" -e $0
  exit 0
fi


# Show executed commands
set -x


# Batch ffx news
$BINDIR/send-ffx

# Batch ffx mail
$LIBDIR/ftnpack -f 242:1000/1 -I %B/out.0f2/morannon

# Gateway
$BINDIR/send-fidogate

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
