#!/bin/sh
#
# $Id: logdaily.sh,v 4.3 1998/11/08 18:27:57 mj Exp $
#
# Daily log processing
#

# run logcheck
if [ -f /var/log/maillog.1.gz ]; then
    zcat /var/log/maillog.1.gz | <BINDIR>/logcheck -n
    zcat /var/log/maillog.1.gz | <BINDIR>/logcheck -r -m admin-logs
fi

# run logsendmail
if [ -f /var/log/maillog.1.gz ]; then
    zcat /var/log/maillog.1.gz | <BINDIR>/logsendmail -m admin-logs
fi

# run logstat
if [ -f <LOGDIR>/log-in.1.gz ]; then
    zcat <LOGDIR>/log-in.1.gz \
    | <BINDIR>/logstat -m admin-logs -t '(inbound)'
fi
if [ -f <LOGDIR>/log-out.1.gz ]; then
    zcat <LOGDIR>/log-out.1.gz \
    | <BINDIR>/logstat -m admin-logs -t '(news gateway output)'
fi

# run logreport
if [ -f <LOGDIR>/log-in.1.gz ]; then
    zcat <LOGDIR>/log-in.1.gz \
    | <BINDIR>/logreport -n
fi
