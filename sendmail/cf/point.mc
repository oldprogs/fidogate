#
# $Id: point.mc,v 4.0 1996/04/17 18:17:32 mj Exp $
#
# sendmail V8 configuration for a FIDO point, routing all Internet mail
# via FIDOGATE and a FIDO-Internet gateway
#
# $Log: point.mc,v $
# Revision 4.0  1996/04/17 18:17:32  mj
# Initial revision
#

include(`../m4/cf.m4')
VERSIONID(`$Id: point.mc,v 4.0 1996/04/17 18:17:32 mj Exp $')
OSTYPE(linux)dnl

define(`confMIME_FORMAT_ERRORS', `False')
define(`confUSE_ERRORS_TO', `True')

FEATURE(notsticky)dnl
FEATURE(always_add_domain)dnl
FEATURE(nodns)dnl
FEATURE(nocanonify)dnl
FEATURE(nouucp)dnl

MAILER(local)dnl
MAILER(uucp)dnl
MAILER(ftn)dnl

define(`SMART_HOST', ftni:INSERT-YOUR-UPLINK-HERE)


LOCAL_CONFIG
# More trusted users
Tnews
