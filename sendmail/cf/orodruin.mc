#:ts=8
#
# $Id: orodruin.mc,v 4.0 1996/04/17 18:17:32 mj Exp $
#
# orodruin.Fido.DE
#

include(`../m4/cf.m4')
VERSIONID(`$Id: orodruin.mc,v 4.0 1996/04/17 18:17:32 mj Exp $')
OSTYPE(linux)dnl

define(`VERSION_NUMBER', `orodruin-1.0')

define(`confMIME_FORMAT_ERRORS', `False')
define(`confUSE_ERRORS_TO', `True')
define(`confMAX_HOP', `30')
define(`confMESSAGE_TIMEOUT', `5d/2d')

FEATURE(notsticky)dnl
FEATURE(always_add_domain)dnl
FEATURE(nodns)dnl
FEATURE(nocanonify)dnl

MAILER(local)dnl
MAILER(smtp)dnl
MAILER(ftn)dnl
MAILER(ffx)dnl

# Smart host and mailer
define(`SMART_HOST', ffx:morannon.fido.de)


LOCAL_CONFIG
# More trusted users
Tnews
