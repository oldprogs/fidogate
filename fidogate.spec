Summary: Fido-Internet Gateway and Fido Tosser
Name: fidogate
Version: 4.4.0
Release: 1
Copyright: GPL
Group: System Environment/Daemons
Source: ftp://ftp.fido.de/pub/fidogate/fidogate-4.4.0.tar.gz
BuildRoot: /var/tmp/fidogate-root

%description
FIDOGATE Version 4
 * Fido-Internet Gateway
 * Fido FTN-FTN Gateway
 * Fido Mail Processor
 * Fido File Processor
 * Fido Areafix/Filefix

%prep
%setup -q 

%build
make DEBUG=-O2

%install
rm -rf $RPM_BUILD_ROOT

make PREFIX=$RPM_BUILD_ROOT install-dirs
make PREFIX=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc ANNOUNCE COPYING README TODO doc/fidogate.txt doc/gatebau94.txt
%doc doc/gatebau/msgid.txt
%doc doc/*.html doc/html/*.html doc/gatebau/*.html
/etc/fidogate
/usr/lib/fidogate/bin/ftnafutil
/usr/lib/fidogate/bin/runffx
/usr/lib/fidogate/bin/send-ffx
/usr/lib/fidogate/bin/rungate
/usr/lib/fidogate/bin/send-fidogate
/usr/lib/fidogate/bin/ftntickpost
/usr/lib/fidogate/bin/runtoss
/usr/lib/fidogate/bin/rununpack
/usr/lib/fidogate/bin/runin
/usr/lib/fidogate/bin/runmail
/usr/lib/fidogate/bin/runnews
/usr/lib/fidogate/bin/runout
/usr/lib/fidogate/bin/ftnfattach
/usr/lib/fidogate/bin/sumcrc
/usr/lib/fidogate/bin/ftnoutpkt
/usr/lib/fidogate/bin/pktdebug
/usr/lib/fidogate/bin/pktmore
/usr/lib/fidogate/bin/logsendmail
/usr/lib/fidogate/bin/logreport
/usr/lib/fidogate/bin/logstat
/usr/lib/fidogate/bin/logdaily
/usr/lib/fidogate/bin/logcheck
/usr/lib/fidogate/bin/runchklock
/usr/lib/fidogate/bin/areasbbssync
/usr/lib/fidogate/bin/recvuu
/usr/lib/fidogate/bin/senduu
/usr/lib/fidogate/bin/senduumail
/usr/lib/fidogate/bin/latest
/usr/lib/fidogate/bin/nl-autoupd
/usr/lib/fidogate/bin/nl-del
/usr/lib/fidogate/bin/nl-diff
/usr/lib/fidogate/bin/nl-check
/usr/lib/fidogate/bin/out-manip
/usr/lib/fidogate/bin/out-rm0
/usr/lib/fidogate/bin/out-rmbsy
/usr/lib/fidogate/charset.bin
/usr/lib/fidogate/charset.map
/usr/lib/fidogate/ftnaf
/usr/lib/fidogate/ftnafpkt
/usr/lib/fidogate/ftnafmail
/usr/lib/fidogate/areafix.help
/usr/lib/fidogate/ffx
/usr/lib/fidogate/ffxqt
/usr/lib/fidogate/ffxmail
/usr/lib/fidogate/ffxrmail
/usr/lib/fidogate/ffxnews
/usr/lib/fidogate/ftnmail
/usr/lib/fidogate/rfc2ftn
/usr/lib/fidogate/ftn2rfc
/usr/lib/fidogate/ftnin
/usr/lib/fidogate/ftninpost
/usr/lib/fidogate/ftninrecomb
/usr/lib/fidogate/ftntick
/usr/lib/fidogate/ftnhatch
/usr/lib/fidogate/ftn2ftn
/usr/lib/fidogate/ftntoss
/usr/lib/fidogate/ftnexpire
/usr/lib/fidogate/ftnroute
/usr/lib/fidogate/ftnpack
/usr/lib/fidogate/ftnbsy
/usr/lib/fidogate/ftnconfig
/usr/lib/fidogate/ftnseq
/usr/lib/fidogate/ftnflo
/usr/lib/fidogate/ftnlock
/usr/lib/fidogate/ftnlog
/usr/info/fidogate.info
/usr/info/fidogate.info-1
/usr/info/fidogate.info-2
/usr/info/fidogate.info-3
/var/log/fidogate
/var/lib/fidogate
/var/lib/fidogate/seq
/var/lock/fidogate
/var/spool/fidogate
/var/spool/fidogate/outrfc
/var/spool/fidogate/outrfc/mail
/var/spool/fidogate/outrfc/news
/var/spool/fidogate/outpkt
/var/spool/fidogate/outpkt/mail
/var/spool/fidogate/outpkt/news
/var/spool/fidogate/toss
/var/spool/fidogate/toss/toss
/var/spool/fidogate/toss/route
/var/spool/fidogate/toss/pack
/var/spool/fidogate/toss/bad
/var/spool/bt
/var/spool/bt/tick
/var/spool/bt/ffx
/var/spool/bt/in
/var/spool/bt/pin
/var/spool/bt/uuin
/var/spool/bt/ftpin

%changelog
* Sun Jan 30 2000 Martin Junius <mj@m-j-s.net>
- first try at rpm spec file for 4.4.0
