#!/bin/sh
#
# $Id: senduu.sh,v 4.3 1998/01/20 21:47:44 mj Exp $
#
# Send stuff on hold as UUENCODEd mail
#
# usage: senduu user@domain Z:N/F.P ...
#

PRG=<LIBDIR>

if [ $# -ne 2 ]; then
  echo "usage: senduu user@domain Z:N/F.P ..."
  exit 1
fi

user=$1
shift

for node in $*; do
    $PRG/ftnflo -x "$PRG/senduumail $user %s" $node
done
