#!/bin/sh
#
# Remove BSY files in outbound, older than 4 hours
#
# Needs GNU find!
#

OUT="/var/spool/bt/out /var/spool/bt/out.0f2 /var/spool/bt/out.0fe"

find $OUT -follow -name '*.bsy' -mmin +240 -print -exec rm -f {} \; 2>/dev/null
