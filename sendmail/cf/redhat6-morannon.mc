#
# $Id: redhat6-morannon.mc,v 4.6 2001/07/27 21:00:16 mj Exp $
#
# Fido.DE (morannon.fido.de)
#

include(`../m4/cf.m4')

VERSIONID(`$Id: redhat6-morannon.mc,v 4.6 2001/07/27 21:00:16 mj Exp $')

dnl #
dnl # Configuration
dnl #
define(`confCF_VERSION', `redhat6-morannon-4.6')
define(`confMIME_FORMAT_ERRORS', `False')
define(`confUSE_ERRORS_TO', `True')
define(`confMAX_HOP', `30')
define(`confMESSAGE_TIMEOUT', `5d/2d')
define(`confAUTO_REBUILD',true)
define(`confTO_CONNECT', `1m')
define(`confDOMAIN_NAME', `morannon.fido.de')
dnl # we're sending bounce mails not postmaster to avoid flooding the
dnl # postmaster mailbox, postmaster-errors points to /dev/null
define(`confDOUBLE_BOUNCE_ADDRESS', `postmaster-errors')
define(`confCOPY_ERRORS_TO', `postmaster-errors')

dnl # RedHat specific
define(`confDEF_USER_ID',``8:12'')
dnl # It's a Linux box ;-)
OSTYPE(`linux')

define(`PROCMAIL_MAILER_PATH',`/usr/bin/procmail')
dnl # extra aliases file for majordomo
define(`ALIAS_FILE',`/etc/aliases,/etc/majordomo.aliases')

FEATURE(`smrsh',`/usr/sbin/smrsh')
FEATURE(`virtusertable',`hash -o /etc/mail/virtusertable')
FEATURE(redirect)
FEATURE(always_add_domain)
FEATURE(use_cw_file)
FEATURE(local_procmail)
FEATURE(`access_db')
FEATURE(`blacklist_recipients')
FEATURE(mailertable,hash /etc/mail/mailertable.db)

dnl # RBL
dnl # To use multiple rbl's, you *must* apply mrbl.p3 to the original
dnl # sendmail-cf files in /usr/lib/sendmail-cf
dnl # See also http://www.sendmail.org/~ca/email/chk-89n.html

dnl # mail-abuse is no longer free, starting 2001-08-01
dnl FEATURE(rbl, `rbl.maps.vix.com', `Rejected - see http://www.mail-abuse.org/rbl/')
dnl FEATURE(rbl, `dul.maps.vix.com', `Dialup - see http://www.mail-abuse.org/dul/')
dnl FEATURE(rbl, `relays.mail-abuse.org', `Open spam relay - see http://www.mail-abuse.org/rss')

dnl # some ORBS sucessors
FEATURE(rbl, `or.orbl.org', `Open relay - see http://www.orbl.org/')
FEATURE(rbl, `inputs.orbz.org', `Open relay - see http://www.orbz.org/')
FEATURE(rbl, `outputs.orbz.org', `Open relay - see http://www.orbz.org/')
FEATURE(rbl, `relays.ordb.org', `Open relay - see http://www.ordb.org/')


MAILER(procmail)
MAILER(smtp)
MAILER(ftn)
MAILER(ffx)


# Alternate hostnames
Cwmorannon-ftn.fido.de morannon-ftn
Cwsungate-ftn.fido.de sungate-ftn
Cwsungate.fido.de sungate
Cwfido.de
Cwmorannon.faho.rwth-aachen.de
Cwmorannon.fido.de morannon
Cwwww.fido.de www
Cwftp.fido.de ftp
Cwgate.fido.de gate

LOCAL_CONFIG
# More trusted users
Tnews
Tmajordomo

LOCAL_RULE_3

LOCAL_NET_CONFIG
