#!/bin/sh
#
# $Id: logdaily.sh,v 4.2 1998/03/22 17:57:34 mj Exp $
#
# Daily log maintenance
#
LIBDIR=<LIBDIR>

# run logcheck
if [ -f /var/log/maillog.1.gz ]; then
    zcat /var/log/maillog.1.gz | $LIBDIR/logcheck -n
    zcat /var/log/maillog.1.gz | $LIBDIR/logcheck -r -m admin-logs
fi

# run logsendmail
if [ -f /var/log/maillog.1.gz ]; then
    zcat /var/log/maillog.1.gz | $LIBDIR/logsendmail -m admin-logs
fi

