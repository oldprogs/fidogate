#!/bin/sh
#
# $Id: rc.fidonet.sh,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# FIDOGATE boot cleanup
#

SPOOL=<SPOOLDIR>


# Remove lock files
rm -f $SPOOL/locks/*

# Remove .tmp files
rm -f $SPOOL/out/*.tmp
rm -f $SPOOL/toss/*/*.tmp
