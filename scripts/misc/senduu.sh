#!/bin/sh
#
# $Id: senduu.sh,v 4.1 1996/12/29 12:16:39 mj Exp $
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

$PRG/ftnflo -x "$PRG/senduumal $user %s" $node
