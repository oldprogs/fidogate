#!/bin/sh
#
# Remove BSY files in outbound, older than 4 hours
#
# Needs GNU find!
#

OUT="<BTBASEDIR>/out*"

find $OUT -follow -name '*.bsy' -mmin +240 -print -exec rm -f {} \; 2>/dev/null
