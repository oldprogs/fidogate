#!/bin/sh
#
# $Id: runpoll-binkd.sh,v 4.1 1999/02/07 11:05:22 mj Exp $
#
# Poll uplink
#

LIBDIR=<LIBDIR>
BINDIR=<BINDIR>
NEWS=<NEWSETCDIR>

BINKD=/usr/sbin/binkd
BINKDCFG=/etc/binkd/binkd.cfg


# -xterm: run in XTerm window
if [ "$1" = "-xterm" ]; then
  exec xterm -display :0 -g 80x20 -title "FIDOGATE runpoll (BinkD)" -e $0
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
$BINKD -p $BINKDCFG

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
