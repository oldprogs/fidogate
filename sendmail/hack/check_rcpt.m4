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
VERSIONID(`@(#)check_rcpt.m4	1.0 (Claus Assmann) 1996-11-23')


divert(2)
LOCAL_RULESETS
Scheck_rcpt
R$+			$: $>3 $1				canonify
R$+			$: $(dequote $1 $)
R$-			$@ ok					to here
R$* % $*		$: relay attempt
R$* @ $* < @ $*.>	$: relay attempt
R$+ < @ $=w.>		$@ ok					to here
ifdef(`_USE_RELAYTO_',`',`#')dnl
R$+ < @ $*$={RelayTo}.>	$@ ok		allow relay to there
define(`COMMENT', ifdef(`_USE_IP_', `', `#'))dnl
COMMENT`'R$+			$: $(dequote "" $&{client_addr} $)
COMMENT`'R0			$@ ok		client_addr is 0 for sendmail -bs
COMMENT`'R$={LocalIP}.$*		$@ ok					from here
undefine(`COMMENT')dnl
define(`COMMENT', ifdef(`_USE_NAMES_', `', `#'))dnl
COMMENT`'R$+			$: $(dequote "" $&{client_name} $)
COMMENT`'R$-			$@ ok					from here
COMMENT`'R$*$={LocalNames}		$@ ok					from here
undefine(`COMMENT')dnl
ifdef(`_USE_RCPT_', `', `errprint(`HACK "check_rcpt" requires use_ip or use_names')')
R$*			$#error $@ 5.7.1 $: "551 we do not support relaying"

