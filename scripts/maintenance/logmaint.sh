#!/bin/sh
#
# $Id: logmaint.sh,v 4.1 1997/04/19 11:41:48 mj Exp $
#
# Cycle FIDOGATE log files, gzip old ones
#

# Permissions for new log files
PERM=644
# Directory with syslog files
LOGDIR=<LOGDIR>
# FIDOGATE scripts
PRG=<LIBDIR>
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
cycle $LOGDIR/log           2 1 0
cycle $LOGDIR/log-in
cycle $LOGDIR/log-out
cycle $LOGDIR/ifmail.debug
cycle $LOGDIR/ifmail.log      1 0

# Run statistics
if [ -f $LOGDIR/log-in.0 -a -x $PRG/logstat ]; then
  $PRG/logstat -m admin -t '(inbound)'        $LOGDIR/log-in.0
fi
if [ -f $LOGDIR/log-out.0 -a -x $PRG/logstat ]; then
  $PRG/logstat -m admin -t '(gateway output)' $LOGDIR/log-out.0
fi
if [ -f $LOGDIR/log.0 -a -x $PRG/logreport ]; then
  $PRG/logreport -n $LOGDIR/log.0
fi
if [ -f $LOGDIR/log-in.0 -a -x $PRG/logreport ]; then
  $PRG/logreport -n $LOGDIR/log-in.0
fi

# Compress old log files
compr $LOGDIR/log
compr $LOGDIR/log-in
compr $LOGDIR/log-out
compr $LOGDIR/ifmail.debug
compr $LOGDIR/ifmail.log
