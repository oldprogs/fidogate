divert(-1)
#
# Copyright (c) 1997 Claus Assmann <ca@informatik.uni-kiel.de>
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
VERSIONID(`@(#)check_relay.m4	3.0 (Claus Assmann) 1997-06-01')
PUSHDIVERT(6)
ifdef(`_JUNK_DEFINED_MAIL',,
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
define(`_JUNK_DEFINED_RELAY',1)
)
POPDIVERT

divert(2)
LOCAL_RULESETS
SjunkIP
# lookup IP in database
# full IP address
R$-.$-.$-.$-	$: $(junk $1.$2.$3.$4 $)
# class C net
R$-.$-.$-.$-	$: $(junk $1.$2.$3 $: $1.$2.$3.$4 $)
# class B net
R$-.$-.$-.$-	$: $(junk $1.$2 $: $1.$2.$3.$4 $)
# class A net
R$-.$-.$-.$-	$: $(junk $1 $: $1.$2.$3.$4 $)

Scheck_relay
# check IP
R$+ $| $+	$: $1 $| $>junkIP $2
R$+ $| $*@JUNK	$#error $@ 5.7.1 $: $2
# check hostname
R$+ $| $+	$: $>junk <@$1>
R$*<@$*@JUNK>$*	$#error $@ 5.7.1 $: $2
