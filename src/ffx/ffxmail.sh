#!/bin/sh
#
# $Id: ffxmail.sh,v 4.1 1996/04/22 20:02:04 mj Exp $
#
# Frontend for delivering mail via ffx/ffxqt
#
# Customize case statement for your system!!
#

if [ $# -lt 2 ] ;then
	echo "usage: ffxmail Z:N/F.P|HOST user@domain ..."
	exit 1
fi

node="$1"
shift
addr="$*"

nocmpr="-n";

case $node in

	242:1000/4 | *.dfv.rwth-aachen.de | *.comnets.rwth-aachen.de)
		batch="-b tweety"
		faddr="242:1000/4"
		;;

	242:1000/5 | orodruin.fido.de | orodruin.fido.de.)
		batch="-b orodruin"
		faddr="242:1000/5"
		;;

	242:4900/99 | fido.de | morannon.fido.de)
		batch="-b morannon"
		faddr="242:4900/99"
		;;

##### Insert other nodes here ... #####
#	xyz)
#		batch="..."
#		faddr="..."
#		;;

	# Unknown node
	*)
		echo "ffxmail: unknown node $node"
		# EX_NOHOST
		exit 68
		;;

esac

exec <LIBDIR>/ffx $nocmpr $batch -- $faddr rmail $addr
