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
VERSIONID(`@(#)check_mail.m4	2.2 (Claus Assmann) 1997-09-25')
PUSHDIVERT(6)
ifdef(`_JUNK_DEFINED_RELAY',,
# file containing full e-mail addresses of spammers (for check_mail):
# spammer@address.domain SPAMMER
# or junk domains (for check_mail, check_relay):
# junk.domain JUNK
# or IP addresses (for check_relay):
# D.X.Y.Z IP
# C.X.Y IP
# B.X IP
# A IP
Kjunk ifelse(_ARG_, `', `dbm /etc/mail/junk', `_ARG_')
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
R$*<@$+>$*		$:$1<@$(junk $2$)>$3
# exists? return
R$*<@JUNK>$*		$@$1<@JUNK>$2
# lookup address in database
R$*<@$+>$*		$:$1<@$(junk $1@$2 $:$2$)>$3
# exists? return
R$*<@SPAMMER>$*		$@$1<@SPAMMER>$2
# mv one subdomain in front, try again
R$*<@$-.$-.$+>$*	$: $2<@> $>junk $1<@$3.$4>$5
# undo damage
R$*<@>$*<@$+>$*		$2<@$1.$3>$4

Scheck_mail
# don't check these
R<$*@$=w>	$@ ok			shortcut
# idea from Steven Schultz
R<>		$: <$n @ $(dequote "" $&{client_name} $) >
ifdef(`_ACCEPT_SOME_',
# accept some stupid stuff
R<$*@$+>	$:<$1@$(accept $2 $:$2$)>
R<$*@OK>	$@ ok)
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
undefine(`COMMENT')dnl
# copy the result of the lookup
R$*		$:$1 $| $1
# now remove the dot
R$* $| $*<@$*.>$*	$: $1 $| $2<@$3>$4
# and check the database
R$* $| $*<@$*>$*	$: $1 $| $>junk $2<@$3>
R$* $| $*<@$*JUNK>$*	$#error $@ 5.7.1 $: "551 This domain is banned, contact your local admin." 	go away
R$* $| $*<@$*SPAMMER>$*	$#error $@ 5.7.1 $: "551 You are banned, contact your local admin." 	go away
# restore original value (after canonicalization by ruleset 3)
R$* $| $*		$: $1
undefine(`COMMENT')dnl
define(`COMMENT', ifdef(`_DNSVALID_', `', `#'))dnl
# this is dangerous! no real name
COMMENT`'R$*<@$*$~P>$*	$#error $@ 4.1.8 $: 451 unresolvable host name $2$3, check your setup.
divert(0)
