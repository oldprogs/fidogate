#:ts=8
#
# $Id: orodruin-smtp.mc,v 4.1 1996/09/22 12:23:32 mj Exp $
#
# Fido.DE subsidiary machine (SMTP)
#
# $Log: orodruin-smtp.mc,v $
# Revision 4.1  1996/09/22 12:23:32  mj
# See ChangeLog
#
# Revision 4.0.0.1  1996/04/17 18:17:32  mj
# FIDOGATE 4.0 import
#
# Revision 1.5  1995/08/01  08:01:28  mj
# Set MAX_HOP to 30, MESSAGE_TIMEOUT to 5d/2d
#
# Revision 1.0  1995/06/05  08:45:24  mj
# Initial revision
#
# Revision 1.0  1995/06/05  08:45:24  mj
# Initial revision
#
#

include(`../m4/cf.m4')
VERSIONID(`$Id: orodruin-smtp.mc,v 4.1 1996/09/22 12:23:32 mj Exp $')
OSTYPE(linux)dnl

define(`confCF_VERSION', `orodruin-4.1')

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

# Smart host and mailer
define(`SMART_HOST', smtp:morannon.)


LOCAL_CONFIG
# More trusted users
Tnews
