PUSHDIVERT(-1)
#
# FIDOGATE FFX mailer for sendmail 8.6.9
#
# MAILER(smtp) and MAILER(uucp) must be included!
#

ifdef(`FFX_MAILER_PATH',, `define(`FFX_MAILER_PATH', /usr/local/lib/fidogate/ffxmail)')
ifdef(`FFX_MAILER_ARGS',, `define(`FFX_MAILER_ARGS', `ffxmail $h $u')')
ifdef(`FFX_MAILER_FLAGS',, `define(`FFX_MAILER_FLAGS', `')')
POPDIVERT
#####################################
###    FFX Mailer specification   ###
#####################################

VERSIONID(`$Revision: 4.0 $')

ifdef(`_MAILER_smtp_',
`# FIDOGATE mailer
Mffx,	P=FFX_MAILER_PATH, F=CONCAT(mDFMhu, FFX_MAILER_FLAGS), S=52/31, R=ifdef(`_ALL_MASQUERADE_', `11/31', `21'),
	A=FFX_MAILER_ARGS
')
