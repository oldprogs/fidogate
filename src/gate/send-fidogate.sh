#!/bin/sh
#
# $Id: send-fidogate.sh,v 4.3 2003/06/08 21:01:26 n0ll Exp $
#
# SH script to send batches to FIDOGATE
#

# Output to "log-news" log file
FIDOGATE_LOGFILE="%G/log-news"
export FIDOGATE_LOGFILE

. <NEWSLIBDIR>/innshellvars

RFC2FTN="<LIBDIR>/rfc2ftn"

PROGNAME=`basename $0`
LOCK=${LOCKS}/LOCK.${PROGNAME}
LOG=${MOST_LOGS}/${PROGNAME}.log

MAXJOBS=200

##  Start logging.
test ! -f ${LOG} && touch ${LOG}
chmod 0660 ${LOG}
exec >>${LOG} 2>&1
echo "${PROGNAME}: [$$] begin `date`"
cd ${BATCH}

##  Anyone else there?
trap 'rm -f ${LOCK} ; exit 1' 1 2 3 15
shlock -p $$ -f ${LOCK} || {
    echo "${PROGNAME}: [$$] locked by [`cat ${LOCK}`]"
    exit 0
}

##  Who are we sending to?
if [ -n "$1" ] ; then
    LIST="$*"
else
    LIST="fidogate"
fi


##  Do the work...
for SITE in ${LIST}; do

    ##  See if any data is ready for host.
    BATCHFILE=${SITE}.fidogate
    if [ -f ${SITE}.work ] ; then
	cat ${SITE}.work >>${BATCHFILE}
	rm -f ${SITE}.work
    fi
    mv ${SITE} ${SITE}.work
    ctlinnd -s -t30 flush ${SITE} || continue
    cat ${SITE}.work >>${BATCHFILE}
    rm -f ${SITE}.work
    if [ ! -s ${BATCHFILE} ] ; then
	echo "${PROGNAME}: [$$] no articles for ${SITE}"
	rm -f ${BATCHFILE}
	continue
    fi

    echo "${PROGNAME}: [$$] begin ${SITE}"

    case $VERSION in
    INN 2.2*)
	# optimized version for INN 2.2, no longer works with INN 2.3
	time $RFC2FTN -f $BATCHFILE -m 500
	;;
    *)
	# generic version using batcher
	time batcher -b500000 -p"$RFC2FTN -b -n" ${SITE} ${BATCHFILE}
	;;
    esac

    echo "${PROGNAME}: [$$] end ${SITE}"
done

##  Remove the lock file.
rm -f ${LOCK}

echo "${PROGNAME}: [$$] end `date`"
