#!/bin/sh
#
# $Id: runpoll-binkd.sh,v 4.6 2000/04/19 17:07:28 mj Exp $
#
# orodruin.fido.de's poll script using binkd
#

LIBDIR=<LIBDIR>
BINDIR=<BINDIR>

BINKD=/usr/sbin/binkd
BINKDCFG=/etc/binkd/binkd.cfg

XTERM=/usr/X11R6/bin/xterm

DUMMYADDR="nobody@fido.de"

# -xterm: run in XTerm window
if [ "$1" = "-xterm" ]; then
  exec $XTERM -display :0 -g 100x20 -title "FIDOGATE runpoll (BinkD)" -e $0
  exit 0
fi


# Show executed commands
set -x

# Send dummy mail for polling
mail -s "POLL" $DUMMYADDR <<EOF
POLL
EOF
sleep 2

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
