#!/bin/sh
#
# $Id: rc.fidogate.sh,v 4.1 1997/06/28 16:26:31 mj Exp $
#
# FIDOGATE boot cleanup
#

SPOOL=<SPOOLDIR>


# Remove lock files
rm -f $SPOOL/locks/*

# Remove .tmp files
rm -f $SPOOL/out/*.tmp
rm -f $SPOOL/toss/*/*.tmp
