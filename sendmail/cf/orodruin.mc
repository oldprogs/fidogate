#:ts=8
#
# $Id: orodruin.mc,v 4.1 1996/08/25 12:11:15 mj Exp $
#
# orodruin.Fido.DE
#

include(`../m4/cf.m4')
VERSIONID(`$Id: orodruin.mc,v 4.1 1996/08/25 12:11:15 mj Exp $')
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

MAILER(local)dnl
MAILER(smtp)dnl
MAILER(ftn)dnl
MAILER(ffx)dnl

# Smart host and mailer
define(`SMART_HOST', ffx:morannon.fido.de)


# Gateway for FIDO mail (warning: conflicts with FAX relay!)
DForodruin-ftn.fido.de

# FIDO domains via mailer ftni (no leading "."!)
CIz2.fidonet.org

# FIDO domains via mailer ftn (no leading "."!)
CN


LOCAL_CONFIG
# More trusted users
Tnews


LOCAL_RULE_3
# class I/N domains are canonical
R$* < @ $* .$=I > $*		$: $1 < @ $2 .$3. > $4
R$* < @ $* .$=N > $*		$: $1 < @ $2 .$3. > $4


LOCAL_NET_CONFIG
# Fido mail to class I via ftni
R$* <@ $* . $=I . > $*		$#ftni $@$F $:$1<@$2.$3.>$4
# Fido mail to class N via ftn
R$* <@ $* . $=N . > $*		$#ftn $@$F $:$1<@$2.$3.>$4
