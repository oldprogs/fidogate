#!/bin/sh
#
# $Id: logmaint.sh,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Cycle FIDOGATE log files, gzip old ones
#

cd <LOGDIR>

[ -f log     ]    && mv log     log.o
[ -f log-out ]    && mv log-out log-out.o
[ -f log-in  ]    && mv log-in  log-in.o

[ -f log-in.o ]   && <LIBDIR>/logstat -m admin -t '(inbound)'        log-in.o
[ -f log-out.o ]  && <LIBDIR>/logstat -m admin -t '(gateway output)' log-out.o

[ -f log.o ]      && <LIBDIR>/logreport -n log.o
[ -f log-in.o ]   && <LIBDIR>/logreport -n log-in.o

[ -f log.ooo.gz ] && mv log.ooo.gz log.oooo.gz
[ -f log.oo.gz  ] && mv log.oo.gz  log.ooo.gz
[ -f log.o.gz   ] && mv log.o.gz   log.oo.gz

rm -rf log.o.gz log-in.o.gz log-out.o-gz
<BINDIR>/gzip -f9 log.o log-in.o log-out.o
