#!/bin/sh
#
# Poll uplink
#
set -x

FIDOGATE=/usr/local/lib/fidogate
IFMAIL=/usr/local/lib/ifmail
NEWS=/etc/news

UPLINK=f2.n1000.z242


# Batch ffx mail files
$FIDOGATE/ffxbatch -F Normal -w -b morannon 242:1000/1

# Batch ffx news
$NEWS/send-ffx

# Gateway
$NEWS/send-fidogate

# Tosser
$FIDOGATE/runtoss out


# Poll
$IFMAIL/ifcico $UPLINK
#$IFMAIL/ifcico -I$IFMAIL/config-i4 $UPLINK


# Unbatch and process ffx files
$FIDOGATE/ffxrun

# Process tic files
$FIDOGATE/ftntick

# Process mail queue
/usr/sbin/sendmail -q

# Tosser
$FIDOGATE/rununpack pin
$FIDOGATE/runtoss   pin

# Gateway
$FIDOGATE/ftnin -x ftninpost
