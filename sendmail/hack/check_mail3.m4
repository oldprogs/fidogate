divert(-1)
#
# Copyright (c) 1996,1997 Claus Assmann <ca@informatik.uni-kiel.de>
#
# In short: you can do whatever you want with this, but don't blame me!
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
divert(0)
VERSIONID(`@(#)check_mail.m4	3.3 (Claus Assmann) 1997-09-25')
PUSHDIVERT(6)
ifdef(`_JUNK_DEFINED_RELAY',,
# file containing full e-mail addresses of spammers (for check_mail):
# spammer@address.domain "Error-Code Error-Text"
# or junk domains (for check_mail, check_relay):
# junk.domain  "Error-Code Error-Text"
# or IP addresses (for check_relay):
# D.X.Y.Z "Error-Code Error-Text"
# C.X.Y "Error-Code Error-Text"
# B.X "Error-Code Error-Text"
# A "Error-Code Error-Text"
Kjunk ifelse(_ARG_, `', `dbm -a@JUNK /etc/mail/junk', `_ARG_')
define(`_JUNK_DEFINED_MAIL',1)
)
ifdef(`_ACCEPT_SOME_',
Kaccept ifelse(_ACCEPT_SOME_,`1',`dbm -o /etc/mail/accept',`_ACCEPT_SOME_')
)
POPDIVERT

divert(2)
LOCAL_RULESETS
# check for junk domain/spammers
Sjunk
# lookup domain in database
R$*<@$+>		$:$1<@$(junk $2$)>
# exists? return
R$*<@$*@JUNK>		$@$1<@$2@JUNK>
# lookup address in database
R$*<@$+>		$:$1<@$(junk $1@$2 $:$2$)>
# exists? return
R$*<@$*@JUNK>		$@$1<@$2@JUNK>
# remove one subdomain, try again
R$*<@$-.$-.$+>		$: $>junk $1<@$3.$4>

Scheck_mail
# don't check these
R<$*@$=w>	$@ ok			shortcut
# idea from Steven Schultz
R<>		$: <$n @ $(dequote "" $&{client_name} $) >
ifdef(`_ACCEPT_SOME_',dnl
# accept some stupid stuff
R<$*@$+>	$:<$1@$(accept $2 $:$2$)>
R<$*@OK>	$@ ok
# addition from Yar Tikhiy
R<$*@$+>	$:<$1@$(accept $1@$2 $:$2$)>
R<$*@OK>	$@ ok)
ifdef(`_CHECK_FROM_',dnl
# mark address
R$*		$:<@>$1
# is the syntax ok (uses <> and no dot at the end?)
R<@><$*@$*$~.>	$:<$1@$2$3>
# mark still there: error...
R<@>$*		$#error $@ 5.1.8 $: 551 illegal MAIL FROM $1,
# remove at least the dot...
R<$*@$*.>	<$1@$2>)
R$*		$: $>3 $1			canonify
ifdef(`_CHECK_LOCAL_',
# another proposal from Steven Schultz
R$-		$1 <@ $(dequote "" $&{client_name} $)>,
R$-		$@ ok)
# no host without a . in the FQHN ?
R$*<@$->$*	$#error $@ 5.1.8 $: 551 invalid host name $2, check your configuration.
undefine(`COMMENT')dnl
define(`COMMENT', ifdef(`_IP_LOOKUP_', `', `#'))dnl
# lookup IP address (reverse mapping available?)
COMMENT`'R$*<@[$-.$-.$-.$-]>$*	$: $1 < @ $[ [ $2.$3.$4.$5 ] $] > $6 
# copy the result of the lookup
R$*		$:$1 $| $1
# now remove the dot
R$* $| $*<@$*.>$*	$: $1 $| $2<@$3>$4
# and check the database
R$* $| $*<@$*>$*	$: $1 $| $>junk $2<@$3>
# match: return given error code (rhs of map)
R$* $| $*<@$*@JUNK>$*	$#error $@ 5.7.1 $: $3
# restore original value (after canonicalization by ruleset 3)
R$* $| $*		$: $1
undefine(`COMMENT')dnl
define(`COMMENT', ifdef(`_DNSVALID_', `', `#'))dnl
# this is dangerous! no real name
# (see RFC 1123,sections 5.2.2 and 5.2.18)
COMMENT`'R$*<@$*$~P>$*	$#error $@ 4.1.8 $: 451 unresolvable host name $2$3, check your setup.
undefine(`COMMENT')dnl
divert(0)
