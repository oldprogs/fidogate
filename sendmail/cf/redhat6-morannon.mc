#
# $Id: redhat6-morannon.mc,v 4.7 2001/10/21 20:17:21 mj Exp $
#
# Fido.DE (morannon.fido.de) - sendmail 8.11.6
#

include(`../m4/cf.m4')

VERSIONID(`$Id: redhat6-morannon.mc,v 4.7 2001/10/21 20:17:21 mj Exp $')

dnl #
dnl # Configuration
dnl #
define(`confCF_VERSION', `morannon-4.7')
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

dnl # some ORBS sucessors
FEATURE(dnsbl, `or.orbl.org', `Open relay - see http://www.orbl.org/')
FEATURE(dnsbl, `inputs.orbz.org', `Open relay - see http://www.orbz.org/')
FEATURE(dnsbl, `outputs.orbz.org', `Open relay - see http://www.orbz.org/')
FEATURE(dnsbl, `relays.ordb.org', `Open relay - see http://www.ordb.org/')


MAILER(smtp)
MAILER(procmail)
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
