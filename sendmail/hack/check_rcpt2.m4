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
VERSIONID(`@(#)check_rcpt2.m4	2.0 (Claus Assmann) 1997-03-01')

divert(2)
LOCAL_RULESETS
Scheck_rcpt
undefine(`COMMENT')dnl
define(`COMMENT', ifdef(`_USE_NAMES_', `', `#'))dnl
COMMENT`'# first: get client name
COMMENT`'R$+			$: $(dequote "" $&{client_name} $) $| $1
COMMENT`'R $| $*			$@ ok			no client name: directly invoked
COMMENT`'#R$- $| $*		$@ ok			for those without full DNS...
COMMENT`'R$*$=w $| $*		$@ ok			from here
ifdef(`_USE_RELAYTO_',`',`#')dnl
R$*$={RelayTo} $| $*		$@ ok		from allowed system
undefine(`COMMENT')dnl
define(`COMMENT', ifdef(`_USE_IP_', `', `#'))dnl
COMMENT`'# first: get client address
COMMENT`'R$+			$: $(dequote "" $&{client_addr} $) $| $1
COMMENT`'R0 $| $*			$@ ok			client_addr is 0 for sendmail -bs
COMMENT`'R$={LocalIP}$* $| $*	$@ ok			from here
undefine(`COMMENT')dnl
ifdef(`_USE_RCPT_', `', `errprint(`HACK "check_rcpt" requires use_ip or use_names')')
# now check other side
R$* $| $*		$: $>3 $2
# remove local part, maybe repeatedly
R$*<@$=w.>$*		$>3 $1 $3
ifdef(`_USE_RELAYTO_',`',`#')dnl
R$*<@$*$={RelayTo}.>$*		$>3 $1 $3
# still something left?
R$*<@$*>$*		$#error $@ 5.7.1 $: 551 we do not relay
