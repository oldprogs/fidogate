#!/bin/sh
#
# $Id: newsyslog.sh,v 4.4 1997/04/10 18:55:58 mj Exp $
#
# Cycle syslog files
#
# WARNING! There are some buggy crond out in Linux land which get killed
#          by the *second* SIGHUP!
#

# Permissions for new log files
PERM=644
# Directory with syslog files
SYSLOGDIR=/var/log
# Compression program
COMPR="gzip -f9"
EXT=".gz"

# Renamed and compress
# usage: move FILE [N]
move () {
  if [ "$2" = "" ]; then
    o=""
    n=".0"
  else
    o=".$2"
    n="."`expr $2 + 1`
  fi
  fo="$1$o"
  fn="$1$n"
  if [ -f "$fn" ]; then
    rm -f $fn
  fi
  if [ -f "$fn$EXT" ]; then
    rm -f $fn$EXT
  fi
  if [ -f "$fo" ]; then
    mv $fo $fn
  fi
  if [ -f "$fo$EXT" ]; then
    mv $fo$EXT $fn$EXT
  fi
}

# Cycle log file
# usage: cycl FILE N ... 3 2 1
# Last log file kept will be FILE.N+1
cycle () {
  if [ -f $1 ]; then 
    file=$1
    shift

    for n in $*; do
      move $file $n
    done
    move $file

    :> $file
    chmod 644 $file
  fi
}

# Compress log file
# usage: compr FILE
compr () {
  for f in $1.[0-9]; do
    if [ -f $f ]; then
      $COMPR $f
    fi
  done
}

# Cycle files
cycle $SYSLOGDIR/maillog  6 5 4 3 2 1 0
cycle $SYSLOGDIR/syslog         3 2 1 0
cycle $SYSLOGDIR/messages       3 2 1 0
cycle $SYSLOGDIR/cron

# # Run sendmailstat
# PRG=/usr/local/bin/sendmailstat
# STATDIR=/var/log/sendmail
# LOG=/var/log/maillog
# 
# if [ -x $PRG ]; then
#   test -d $STATDIR || mkdir $STATDIR
#   $PRG -l $LOG.0 -o $STATDIR/stat-%D
# fi


# Restart syslog, cron
test -f /etc/syslog.pid     && kill -HUP `cat /etc/syslog.pid`
test -f /var/run/syslog.pid && kill -HUP `cat /var/run/syslog.pid`
test -f /etc/crond.pid      && kill -HUP `cat /etc/crond.pid`
test -f /var/run/crond.pid  && kill -HUP `cat /var/run/crond.pid`

# Compress old log files
compr $SYSLOGDIR/maillog
compr $SYSLOGDIR/syslog
compr $SYSLOGDIR/messages
compr $SYSLOGDIR/cron
