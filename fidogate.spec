Summary: Fido-Internet Gateway and Fido Tosser
Name: fidogate
Version: 4.4.2
Release: 2
Copyright: GPL
Group: System Environment/Daemons
Source: ftp://ftp.fidogate.org/pub/fidogate/fidogate-%{version}.tar.gz
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
mkdir -p $RPM_BUILD_ROOT/etc/news
make PREFIX=$RPM_BUILD_ROOT install
make PREFIX=$RPM_BUILD_ROOT install-config

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc ANNOUNCE COPYING README TODO doc/fidogate.txt doc/gatebau94.txt
%doc doc/gatebau/msgid.txt
%doc doc/*.html doc/html/*.html doc/gatebau/*.html
%dir /etc/fidogate
/usr/lib/fidogate
/usr/info/fidogate.info*
/var/log/fidogate
%dir /var/lib/fidogate
/var/lock/fidogate
/var/spool/fidogate
/var/spool/bt
/etc/news/newsfeeds.fidogate
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
* Sat Nov 25 2000 Martin Junius <mj@m-j-s.net>
- Changed release number to 2.6.2, reflecting package for RedHat 6.2

* Sat Nov 18 2000 Martin Junius <mj@m-j-s.net>
- Updated to version 4.4.2

* Tue Mar  7 2000 Martin Junius <mj@m-j-s.net>
- Updated to version 4.4.1

* Sun Jan 30 2000 Martin Junius <mj@m-j-s.net>
- First try at rpm spec file for 4.4.0
