divert(-1)
#
# Copyright (c) 1996 Claus Assmann <ca@informatik.uni-kiel.de>
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
VERSIONID(`@(#)check_mail.m4	1.1 (Claus Assmann) 1997-03-09')

divert(2)
LOCAL_RULESETS
Scheck_mail
define(`COMMENT', ifdef(`_SPAMMERS_', `', `#'))dnl
# no mails from spammers (user@dom.ain)
COMMENT`'R<$={Spammer}>	$#error $@ 5.7.1 $: "551 We don't accept junk mail." 	go away
COMMENT`'R<$={Spammer}.>	$#error $@ 5.7.1 $: "551 We don't accept junk mail." 	go away
COMMENT`'R$={Spammer}	$#error $@ 5.7.1 $: "551 We don't accept junk mail." 	go away
COMMENT`'R$={Spammer}.	$#error $@ 5.7.1 $: "551 We don't accept junk mail." 	go away
undefine(`COMMENT')dnl
R$*		$: $>3 $1						canonify
R$-		$@ ok							local host
define(`COMMENT', ifdef(`_SPAMDOMS_', `', `#'))dnl
# no spam domains
COMMENT`'R$*<@$*$={SpamDomains}.>$*	$#error $@ 5.7.1 $: "551 This domain is banned." 	go away
COMMENT`'R$*<@$*$={SpamDomains}>$*	$#error $@ 5.7.1 $: "551 This domain is banned." 	go away
undefine(`COMMENT')dnl
# if you enable the last rule, you can disable this one.
# host without a . in the FQHN ?
R$*<@$->$*	$#error $@ 5.1.8 $: 551 invalid host name $2		no real name
# lookup IP address (reverse mapping available?)
# R$*<@[$+]>$*	$: $1 < @ $[ [ $2 ] $] > $3
# no DNS entry? this is dangerous!
# R$*<@$*$~P>$*	$#error $@ 4.1.8 $: 451 unresolvable host name $2$3, check your configuration.	
divert(0)

