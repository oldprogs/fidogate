#:ts=8
#
# $Id: orodruin.mc,v 4.2 1996/09/16 20:13:22 mj Exp $
#
# orodruin.Fido.DE
#

include(`../m4/cf.m4')
VERSIONID(`$Id: orodruin.mc,v 4.2 1996/09/16 20:13:22 mj Exp $')
OSTYPE(linux)dnl

define(`VERSION_NUMBER', `orodruin-4.1')

define(`confMIME_FORMAT_ERRORS', `False')
define(`confUSE_ERRORS_TO', `True')
define(`confMAX_HOP', `30')
define(`confMESSAGE_TIMEOUT', `5d/2d')

FEATURE(notsticky)dnl
FEATURE(always_add_domain)dnl
FEATURE(nodns)dnl
FEATURE(nocanonify)dnl
FEATURE(mailertable,hash /etc/mail/mailertable.db)dnl

MAILER(local)dnl
MAILER(smtp)dnl
MAILER(ftn)dnl
MAILER(ffx)dnl

# Smart host and mailer
define(`SMART_HOST', ffx:morannon.fido.de)

# Map for `LOCAL_RULE_3' rewrite rules
Krewrite hash -o /etc/mail/rewrite


LOCAL_CONFIG
# More trusted users
Tnews


LOCAL_RULE_3
# Rewrite addresses according to rewrite.db map
R$* < @ $* > $*			$: $1 < @ $(rewrite $2 $@ %1 $: $2 $) > $3


LOCAL_NET_CONFIG
