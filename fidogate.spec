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

%changelog
* 2000-01-29 Martin Junius <mj@m-j-s.net>
- first try at rpm spec file for 4.4.0
