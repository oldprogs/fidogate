#!/bin/sh
#
# $Id: rundf.sh,v 4.0 1996/04/17 18:17:42 mj Exp $
#
# Check free disk space for runtoss script
#
# Needs GNU fileutils df! (configure PATH if necessary)
#
# Usage: rundf dir ...
#

PATH=<BINDIR>:$PATH

free=`df -P $* 2>/dev/null  | \
      awk '/^\/dev/ || /^[a-zA-Z0-9-]*:\// { print $4; }'`

if [ -z "$free" ]; then
	echo "ERROR"
else
	echo $free
fi
