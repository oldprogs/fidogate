#!/bin/sh
#
# $Id: senduu.sh,v 4.2 1997/02/09 10:04:22 mj Exp $
#
# Send stuff on hold as UUENCODEd mail
#
# usage: senduu Z:N/F.P user@domain
#

PRG=<LIBDIR>

if [ $# -ne 2 ]; then
  echo "usage: senduu Z:N/F.P user@domain"
  exit 1
fi

node=$1
user=$2

$PRG/ftnflo -x "$PRG/senduumail $user %s" $node
