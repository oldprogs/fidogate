#
# $Id: point.mc,v 4.1 1996/09/22 12:23:49 mj Exp $
#
# sendmail V8 configuration for a FIDO point, routing all Internet mail
# via FIDOGATE and a FIDO-Internet gateway
#
# $Log: point.mc,v $
# Revision 4.1  1996/09/22 12:23:49  mj
# See ChangeLog
#
# Revision 4.0.0.1  1996/04/17 18:17:32  mj
# FIDOGATE 4.0 import
#

include(`../m4/cf.m4')
VERSIONID(`$Id: point.mc,v 4.1 1996/09/22 12:23:49 mj Exp $')
OSTYPE(linux)

define(`confMIME_FORMAT_ERRORS', `False')
define(`confUSE_ERRORS_TO', `True')

FEATURE(notsticky)
FEATURE(always_add_domain)
FEATURE(nodns)
FEATURE(nocanonify)
FEATURE(nouucp)

MAILER(local)
MAILER(smtp)
MAILER(uucp)
MAILER(ftn)

define(`SMART_HOST', ftni:INSERT-YOUR-UPLINK-HERE)


LOCAL_CONFIG
# More trusted users
Tnews
