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
VERSIONID(`@(#)check_rcpt3.m4	2.2 (Claus Assmann) 1997-03-09')

divert(2)
LOCAL_RULESETS
Scheck_rcpt
ifdef(`_USE_RCPT_', `', `errprint(`HACK "check_rcpt" requires use_ip or use_names')')dnl
ifdef(`_USE_NAMES_',
# first: get client name
R$+			$: $(dequote "" $&{client_name} $) $| $1
R $| $*			$@ ok		no client name: directly invoked
#R$- $| $*		$@ ok		for those without full DNS...
R$*$={LocalNames} $| $*	$@ ok		from allowed system
R$*$=w $| $*		$@ ok		from here (instead of LocalNames)
,
# first: get client address
R$+			$: $(dequote "" $&{client_addr} $) $| $1
R0 $| $*		$@ ok		client_addr is 0 for sendmail -bs
R$={LocalIP}$* $| $*	$@ ok		from here
)dnl
# now check other side
R$* $| $*		$: $>3 $2
# remove local part, maybe repeatedly
ifdef(`_USE_RELAYTO_',
R$*<@$*$={RelayTo}.>$*		$>3 $1 $4
,
R$*<@$=w.>$*		$>3 $1 $3
)dnl
# still something left?
R$*<@$+>$*		$#error $@ 5.7.1 $: 551 we do not relay
