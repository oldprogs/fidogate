#:ts=8
#
# $Id: morannon.mc,v 4.1 1996/08/25 12:11:15 mj Exp $
#
# Fido.DE domain gateway sendmail V8 configuration
#
# $Log: morannon.mc,v $
# Revision 4.1  1996/08/25 12:11:15  mj
# See ChangeLog
#
# Revision 4.0.0.1  1996/04/17 18:17:32  mj
# FIDOGATE 4.0 import
#
# Revision 1.5  1995/08/01  08:01:28  mj
# Set MAX_HOP to 30, MESSAGE_TIMEOUT to 5d/2d
#
# Revision 1.4  1995/07/06  19:58:44  mj
# Use map /etc/mail/rewrite for LOCAL_RULE_3 rewrite rules.
#
# Revision 1.3  1995/06/04  16:02:44  mj
# New setup for morannon, addresses for mailer #ftn via class N
#
# Revision 1.2  1995/03/27  19:41:15  mj
# New temporary rerouting.
#
# Revision 1.1  1994/12/19  09:45:00  mj
# Improvements: sungate directly (not via MX), $F for FIDOGATE relay,
# $=I for domains via ftni mailer.
#
# Revision 1.0  1994/12/05  19:38:47  mj
# Initial revision
#

include(`../m4/cf.m4')
VERSIONID(`$Id: morannon.mc,v 4.1 1996/08/25 12:11:15 mj Exp $')
OSTYPE(linux)dnl

define(`VERSION_NUMBER', `Fido.DE-4.1')

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
MAILER(uucp)dnl
MAILER(ftn)dnl
MAILER(ffx)dnl

# Alternate names
Cwmorannon-ftn.fido.de morannon-ftn
Cwsungate-ftn.fido.de sungate-ftn
Cwsungate.fido.de sungate

# Local hosts via SMTP
CSorodruin

# Gateway for FIDO mail (warning: conflicts with FAX relay!)
DFsungate-ftn.fido.de

# FIDO domains via mailer ftni (no leading "."!)
CIz242.fido.de z254.fido.de z2.fidonet.org

# FIDO domains via mailer ftn (no leading "."!)
CNfido.de

# Smart host and mailer
define(`SMART_HOST', ffx:zruty.dfv.rwth-aachen.de)

# Map for `LOCAL_RULE_3' rewrite rules
Krewrite dbm -o /etc/mail/rewrite



LOCAL_CONFIG
# More trusted users
Tnews



LOCAL_RULE_3
# Rewrite addresses according to rewrite.db map
R$* < @ $* > $*			$: $1 < @ $(rewrite $2 $@ %1 $: $2 $) > $3

# local SMTP hosts are canonical
R$* < @ $=S > $*		$: $1 < @ $2 .$m. > $3
R$* < @ $=S . $m > $*		$: $1 < @ $2 .$m. > $3

# class I/N domains are canonical
R$* < @ $* .$=I > $*		$: $1 < @ $2 .$3. > $4
R$* < @ $* .$=N > $*		$: $1 < @ $2 .$3. > $4

# handle addresses in local domain (useful only for a domain gateway!)
R$* < @ $* $m > $*		$: $1 < @ $2 $m. > $3



LOCAL_NET_CONFIG
# Mail to domain only is local (useful only for a domain gateway!)
R$+ < @ $=m . >			$#local $: $1

# Mail to class S hosts directly, not via MX!
R$* <@ $=S . $m . > $*		$#smtp $@$2. $:$1<@$2.$m.>$3

# Fido mail to class I via ftni
R$* <@ $* . $=I . > $*		$#ftni $@$F $:$1<@$2.$3.>$4

# Fido mail to class N via ftn
R$* <@ $* . $=N . > $*		$#ftn $@$F $:$1<@$2.$3.>$4

