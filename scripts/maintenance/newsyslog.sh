#!/bin/sh
#
# $Id: newsyslog.sh,v 4.1 1996/08/25 10:16:06 mj Exp $
#
# Cycle syslog files, run sendmailstat
#

# Permissions for new log files
PERM=644
# Directory with syslog files
SYSLOGDIR=/var/log


# Cycle syslog files
cycle () {
  if [ -f $1 ]; then 
    test -f $1.5 && mv $1.5 $1.6
    test -f $1.4 && mv $1.4 $1.5
    test -f $1.3 && mv $1.3 $1.4
    test -f $1.2 && mv $1.2 $1.3
    test -f $1.1 && mv $1.1 $1.2
    test -f $1.0 && mv $1.0 $1.1
    test -f $1   && mv $1   $1.0

    :> $1
    chmod 644 $1
  fi
}

cycle $SYSLOGDIR/cron
cycle $SYSLOGDIR/maillog
cycle $SYSLOGDIR/messages
cycle $SYSLOGDIR/syslog


# Run sendmailstat
PRG=/usr/local/bin/sendmailstat
STATDIR=/var/log/sendmail
LOG=/var/log/maillog

if [ -x $PRG ]; then
  test -d $STATDIR || mkdir $STATDIR
  $PRG -l $LOG.0 -o $STATDIR/stat-%D
fi


# Restart syslog, cron
test -f /etc/syslog.pid     && kill -HUP `cat /etc/syslog.pid`
test -f /var/run/syslog.pid && kill -HUP `cat /var/run/syslog.pid`
test -f /etc/crond.pid      && kill -HUP `cat /etc/crond.pid`
test -f /var/run/crond.pid  && kill -HUP `cat /var/run/crond.pid`
