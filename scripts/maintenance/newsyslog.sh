#!/bin/sh
#
# $Id: newsyslog.sh,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Cycle syslog files, run sendmailstat
#

LOG=/var/adm/messages
PRG=/usr/local/bin/sendmailstat
STATDIR=/var/adm/sendmail

# Cycle messages
test -f $LOG.5 && mv $LOG.5 $LOG.6
test -f $LOG.4 && mv $LOG.4 $LOG.5
test -f $LOG.3 && mv $LOG.3 $LOG.4
test -f $LOG.2 && mv $LOG.2 $LOG.3
test -f $LOG.1 && mv $LOG.1 $LOG.2
test -f $LOG.0 && mv $LOG.0 $LOG.1
                  mv $LOG   $LOG.0

cp /dev/null $LOG
chmod 644    $LOG

# Run sendmailstat
test -d $STATDIR || mkdir $STATDIR
test -f $PRG     && $PRG -l $LOG.0 -o $STATDIR/stat-%D

# Restart syslog
test -f /etc/syslog.pid     && kill -HUP `cat /etc/syslog.pid`
test -f /var/run/syslog.pid && kill -HUP `cat /var/rum/syslog.pid`
