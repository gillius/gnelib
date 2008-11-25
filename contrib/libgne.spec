%define is_mandrake %(test -e /etc/mandrake-release && echo 1 || echo 0)
%define is_suse %(test -e /etc/SuSE-release && echo 1 || echo 0)
%define is_fedora %(test -e /etc/fedora-release && echo 1 || echo 0)

%define dist redhat
%define disttag rh

%if %is_mandrake
%define dist mandrake
%define disttag mdk
%endif
%if %is_suse
%define dist suse
%define disttag suse
%endif
%if %is_fedora
%define dist fedora
%define disttag fc
%endif

%define distver %(release="`rpm -q --queryformat='%{VERSION}' %{dist}-release 2> /dev/null | tr . : | sed s/://g`" ; if test $? != 0 ; then release="" ; fi ; echo "$release")
%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)


Summary: Game Networking Engine
Name: libgne
Version: 0.75
Release: 1.%{disttag}%{distver}
License: LGPL
Group: Libraries/Networking
URL: http://www.gillius.org/gne/
Source0: gnelib-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Packager: %packer

BuildRequires: boost-devel
BuildRequires: hawknl-devel

Requires: boost
Requires: hawknl

%description 
GNE, or the Game Networking Engine, is a cross-platform,
multithreaded, C++ networking library with an API specifically
addressing the needs of game networking.

%prep
%setup -n gnelib-%{version}

%build
./fixlinux.sh 
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/include
mkdir -p $RPM_BUILD_ROOT/usr/lib
make install LIBDIR=$RPM_BUILD_ROOT/usr/lib/ INCDIR=$RPM_BUILD_ROOT/usr/include/

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%_prefix/include/*
%_prefix/lib/*


%changelog
* Sat Mar 31 2007 Glen Cornell <glen@localhost.localdomain> - 
- Initial build.
