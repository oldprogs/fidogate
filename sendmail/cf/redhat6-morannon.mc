#
# $Id: redhat6-morannon.mc,v 4.2 1999/10/07 18:56:31 mj Exp $
#
# orodruin.Fido.DE
#

include(`../m4/cf.m4')

VERSIONID(`$Id: redhat6-morannon.mc,v 4.2 1999/10/07 18:56:31 mj Exp $')

dnl #
dnl # Configuration
dnl #
define(`confCF_VERSION', `redhat6-morannon-4.2')
define(`confMIME_FORMAT_ERRORS', `False')
define(`confUSE_ERRORS_TO', `True')
define(`confMAX_HOP', `30')
define(`confMESSAGE_TIMEOUT', `5d/2d')
define(`confAUTO_REBUILD',true)
define(`confTO_CONNECT', `1m')

dnl # RedHat specific
define(`confDEF_USER_ID',``8:12'')
dnl # It's a Linux box ;-)
OSTYPE(`linux')

define(`PROCMAIL_MAILER_PATH',`/usr/bin/procmail')
dnl # extra aliases file for majordomo
define(`ALIAS_FILE',`/etc/aliases,/opt/majordomo/majordomo.aliases')

FEATURE(`smrsh',`/usr/sbin/smrsh')
FEATURE(`virtusertable',`hash -o /etc/mail/virtusertable')
FEATURE(redirect)
FEATURE(always_add_domain)
FEATURE(use_cw_file)
FEATURE(local_procmail)
FEATURE(`access_db')
FEATURE(`blacklist_recipients')
FEATURE(mailertable,hash /etc/mail/mailertable.db)

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
