#!/bin/sh
#
# $Id: rununpack.sh,v 4.11 1997/06/21 21:16:47 mj Exp $
#
# Unpack ArcMail archives
#
# Usage: rununpack name
#
# Needs LIBDIR/magic with the following definitions:
#
#  # ----- Archiver ---------------------------------------------------------
#  0	string		PK		ZIP Archive
#  0	byte		0x1a		ARC Archive
#  2	string		-lh		LHA Archive
#  0	string		ZOO		ZOO Archive
#  0	short		0xea60		ARJ Archive
#  0	short		0x60ea		ARJ Archive
#

# Programs are searched in: /bin, /usr/bin, /usr/local/bin

PRG=<LIBDIR>
SPOOL=`$PRG/ftnconfig =spooldir`
OUTBOUND=`$PRG/ftnconfig =outbound`
INBOUND=`$PRG/ftnconfig =inbound`
PINBOUND=`$PRG/ftnconfig =pinbound`
UUINBOUND=`$PRG/ftnconfig =uuinbound`

if [ -z "$SPOOL" ]; then
  echo "runtoss: parameter spooldir missing"
  exit 1
fi
if [ -z "$OUTBOUND" ]; then
  echo "runtoss: parameter outbound missing"
  exit 1
fi
if [ -z "$INBOUND" ]; then
  echo "runtoss: parameter inbound missing"
  exit 1
fi
if [ -z "$PINBOUND" ]; then
  echo "runtoss: parameter pinbound missing"
  exit 1
fi
if [ -z "$UUINBOUND" ]; then
  echo "runtoss: parameter uuinbound missing"
  exit 1
fi


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
[ -d $INPUT/tmpunpack ] || mkdir $INPUT/tmpunpack
cd $INPUT/tmpunpack

[ -d $INPUT/bad ] || mkdir $INPUT/bad


#
# Unpack all mail archives in $INPUT
#
for arc in \
  $INPUT/*.[sS][uU]? $INPUT/*.[mM][oO]? $INPUT/*.[tT][uU]? \
  $INPUT/*.[wW][eE]? $INPUT/*.[tT][hH]? $INPUT/*.[fF][rR]? $INPUT/*.[sS][aA]?
do
if [ -f $arc ]; then

	# Find out archive type and the unpacking command
	type=`file -m $PRG/magic $arc | awk '{print $2}'`
	cmd="unknown"
	case $type in
	ARJ | Arj | arj)
		[ -x /bin/unarj           ] && cmd=/bin/unarj
		[ -x /usr/bin/unarj       ] && cmd=/usr/bin/unarj
		[ -x /usr/local/bin/unarj ] && cmd=/usr/local/bin/unarj
		xcmd="$cmd e $arc"
		lcmd="$cmd l $arc"
		;;
	ARC | Arc | arc)
		[ -x /bin/arc             ] && cmd=/bin/arc
		[ -x /usr/bin/arc         ] && cmd=/usr/bin/arc
		[ -x /usr/local/bin/arc   ] && cmd=/usr/local/bin/arc
		xcmd="$cmd e $arc"
		lcmd="$cmd l $arc"
		;;
	ZIP | Zip | zip)
		[ -x /bin/unzip           ] && cmd=/bin/unzip
		[ -x /usr/bin/unzip       ] && cmd=/usr/bin/unzip
		[ -x /usr/local/bin/unzip ] && cmd=/usr/local/bin/unzip
		xcmd="$cmd -ojL $arc"
		lcmd="$cmd -l   $arc"
                ;;
        RAR | Rar | rar)
                [ -x /bin/rar             ] && cmd=/bin/rar
                [ -x /usr/bin/rar         ] && cmd=/usr/bin/rar
                [ -x /usr/local/bin/rar   ] && cmd=/usr/local/bin/rar
                xcmd="$cmd e $arc"
                lcmd="$cmd l $arc"
		;;
	LHA | LHa | Lha | lha)
		[ -x /bin/lharc           ] && cmd=/bin/lharc
		[ -x /usr/bin/lharc       ] && cmd=/usr/bin/lharc
		[ -x /usr/local/bin/lharc ] && cmd=/usr/local/bin/lharc
		[ -x /bin/lha             ] && cmd=/bin/lha
		[ -x /usr/bin/lha         ] && cmd=/usr/bin/lha
		[ -x /usr/local/bin/lha   ] && cmd=/usr/local/bin/lha
		xcmd="$cmd ei $arc"
		lcmd="$cmd l  $arc"
		;;
	ZOO | Zoo | zoo)
		[ -x /bin/zoo             ] && cmd=/bin/zoo
		[ -x /usr/bin/zoo         ] && cmd=/usr/bin/zoo
		[ -x /usr/local/bin/zoo   ] && cmd=/usr/local/bin/zoo
		xcmd="$cmd e: $arc"
		lcmd="$cmd l  $arc"
		;;
	*)
		echo    "rununpack: unknown archive type"
		file -m $PRG/magic $arc
		echo    "rununpack: moving to $INPUT/bad"
		mv $arc $INPUT/bad
		continue
		;;
	esac

	if [ $cmd = "unknown" ] ; then
	  echo    "rununpack: no archiver program found for type $type"
	  echo    "rununpack: moving to $INPUT/bad"
	  mv $arc $INPUT/bad
	  continue
	fi

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
	rm -f unpack.out

	# Move packet files to tosser input directory
	for f in *; do if [ -f $f ]; then
		ff=$f
		pp=1
		while [ -f ../$ff ]; do
		  ff="$pp$f"
		  pp=`expr $pp + 1`
		done
		mv $f ../$ff
	fi; done

	# Remove arc
	rm -f $arc
			
fi
done


exit 0
