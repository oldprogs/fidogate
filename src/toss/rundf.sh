#!/bin/sh
#
# $Id: rundf.sh,v 4.1 1998/01/24 15:45:56 mj Exp $
#
# Check free disk space for runtoss script
#
# Needs GNU fileutils df! (configure PATH if necessary)
#
# Usage: rundf dir ...
#

free=`<DF> -P $* 2>/dev/null  | \
      awk '/^\/dev/ || /^[a-zA-Z0-9-]*:\// { print $4; }'`

if [ -z "$free" ]; then
	echo "ERROR"
else
	echo $free
fi
