PUSHDIVERT(-1)
#
# $Id: ftn.m4,v 4.1 1996/09/22 12:24:03 mj Exp $
#
# FIDOGATE FTN mailer for sendmail V8
#
# MAILER(smtp) and MAILER(uucp) must be included!
#

ifdef(`FTN_MAILER_PATH',, `define(`FTN_MAILER_PATH', /usr/local/lib/fidogate/rfc2ftn)')
ifdef(`FTN_MAILER_ARGS',, `define(`FTN_MAILER_ARGS', `rfc2ftn $u')')
ifdef(`FTN_MAILER_ARGSI',, `define(`FTN_MAILER_ARGSI', `rfc2ftn -i $u')')
ifdef(`FTN_MAILER_FLAGS',, `define(`FTN_MAILER_FLAGS', `')')
POPDIVERT

#####################################
###    FTN Mailer specification   ###
#####################################

VERSIONID(`$Revision: 4.1 $')

ifdef(`_MAILER_smtp_',
`# FIDOGATE mailer
Mftn,	P=FTN_MAILER_PATH, F=CONCAT(mDFMuX8, FTN_MAILER_FLAGS), S=11/31, R=ifdef(`_ALL_MASQUERADE_', `21/31', `21'),
	A=FTN_MAILER_ARGS
Mftni,	P=FTN_MAILER_PATH, F=CONCAT(mDFMuX8, FTN_MAILER_FLAGS), S=11/31, R=ifdef(`_ALL_MASQUERADE_', `21/31', `21'),
	A=FTN_MAILER_ARGSI
')
