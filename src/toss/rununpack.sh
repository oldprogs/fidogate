#!/bin/sh
#
# $Id: rununpack.sh,v 4.0 1996/04/17 18:17:42 mj Exp $
#
# Unpack ArcMail archives
#
# Usage: rununpack name
#
# Needs the following definitions in /etc/magic:
#
#  # ----- Archiver ---------------------------------------------------------
#  0	string		PK		ZIP Archive
#  0	byte		0x1a		ARC Archive
#  2	string		-lh		LHA Archive
#  0	string		ZOO		ZOO Archive
#  0	short		0xea60		ARJ Archive
#  0	short		0x60ea		ARJ Archive
#

PRG=<LIBDIR>
# Directory with unpacking programs (arc, unzip, lha, unarj)
#BIN=<BINDIR>
BIN=/usr/bin
SPOOLDIR=<SPOOLDIR>
INBOUND=<INBOUND>
PINBOUND=<PINBOUND>
UUINBOUND=<UUINBOUND>


if [ $# -ne 1 ]; then
	echo "usage: rununpack name"
	exit 1
fi

NAME=$1

#
# Set input and grade depending on NAME
#
case X$NAME in
	Xpin)
		INPUT=$PINBOUND
		;;
	Xin)
		INPUT=$INBOUND
		;;
	Xuuin)
		INPUT=$UUINBOUND
		;;
	*)
		echo "rununpack: unknown $NAME"
		exit 1
		;;
esac


if [ ! -d $INPUT ]; then
	echo "rununpack: $INPUT: no such directory"
	exit 1
fi


#
# Create needed directories
#
[ -d $INPUT/tmp ] || mkdir $INPUT/tmp
cd $INPUT/tmp

[ -d $INPUT/bad ] || mkdir $INPUT/bad


#
# Unpack all mail archives in $INPUT
#
for arc in $INPUT/*.su? $INPUT/*.mo? $INPUT/*.tu? $INPUT/*.we? \
           $INPUT/*.th? $INPUT/*.fr? $INPUT/*.sa?
do
if [ -f $arc ]; then

	# Find out archive type and the unpacking command
	type=`file $arc | awk '{print $2}'`
	case $type in
	ARJ)
		xcmd="$BIN/unarj e   $arc"
		lcmd="$BIN/unarj l   $arc"
		;;
	ARC)
		xcmd="$BIN/arc   e   $arc"
		lcmd="$BIN/arc   l   $arc"
		;;
	ZIP)
		xcmd="$BIN/unzip -xj $arc"
		lcmd="$BIN/unzip -l  $arc"
		;;
	LHA)
		xcmd="$BIN/lharc ei  $arc"
		lcmd="$BIN/lharc l   $arc"
		;;
	ZOO)
		xcmd="$BIN/zoo   e:  $arc"
		lcmd="$BIN/zoo   l   $arc"
		;;
	*)
		echo    "rununpack: unknown archive type"
		file $arc
		echo    "rununpack: moving to $INPUT/bad"
		mv $arc $INPUT/bad
		continue
		;;
	esac

	# First, try to list archive, if not o.k. skip it.
	eval $lcmd </dev/null >/dev/null 2>&1
	st=$?
	if [ $st -ne 0 ]; then
		$PRG/ftnlog -p rununpack \
		    "WARNING: skipping archive $arc ($type)"
		continue
	fi	
	
	# Now do the unpacking
	$PRG/ftnlog -p rununpack "archive $arc ($type)"

	eval $xcmd </dev/null >unpack.out 2>&1
	st=$?
	if [ $st -ne 0 ]; then
		echo "rununpack: unpacking failed"
		echo "$xcmd"
		cat unpack.out
		echo "rununpack: removing extracted files"
		rm -f *
		echo "rununpack: moving to $INPUT/bad"
		$PRG/ftnlog -p rununpack \
		    "ERROR: arc unpack failed, moved to $INPUT/bad/$arc"
		mv $arc $INPUT/bad
		continue
	fi

	# Move *.pkt packet files to tosser input directory
	for f in *.[pP][kK][tT]; do if [ -f $f ]; then
		# Kludge if ../$f already exists - FIXME.
		if [ -f ../$f ]; then
			mv $f ../n$f
		else
			mv $f ../$f
		fi
	fi; done

	# Remove arc and output
	rm -f unpack.out
	rm -f $arc
			
fi
done


exit 0
