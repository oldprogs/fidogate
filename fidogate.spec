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
make PREFIX=$PRM_BUILD_ROOT install-config

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc ANNOUNCE COPYING README TODO doc/fidogate.txt doc/gatebau94.txt
%doc doc/gatebau/msgid.txt
%doc doc/*.html doc/html/*.html doc/gatebau/*.html
/etc/fidogate
/usr/lib/fidogate
/usr/info/fidogate.info*
/var/log/fidogate
/var/lib/fidogate
/var/lock/fidogate
/var/spool/fidogate
/var/spool/bt
/etc/news
%config /etc/fidogate/aliases
%config /etc/fidogate/areas
%config /etc/fidogate/fidogate.conf
%config /etc/fidogate/hosts
%config /etc/fidogate/packing
%config /etc/fidogate/passwd
%config /etc/fidogate/routing
%config /var/lib/fidogate/areas.bbs
%config /var/lib/fidogate/fareas.bbs

%changelog
* Sun Jan 30 2000 Martin Junius <mj@m-j-s.net>
- first try at rpm spec file for 4.4.0
