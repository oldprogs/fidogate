#!/bin/sh
#
# $Id: logdaily.sh,v 4.1 1997/10/22 19:19:19 mj Exp $
#
# Daily log maintenance
#
LIBDIR=<LIBDIR>

# run logcheck
if [ -f /var/log/maillog.1.gz ]; then
    zcat /var/log/maillog.1.gz | $LIBDIR/logcheck -n
fi

