#!/bin/sh
#
# Remove empty ArcMail archives in outbound
#

OUTBOUND=/usr/spool/bt/out*
FIND=/usr/local/bin/find

$FIND $OUTBOUND -type f -size 0c -name '*.??[0-9]' -exec rm -f {} \; -print
