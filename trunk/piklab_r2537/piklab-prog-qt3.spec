%define name     piklab-prog-qt3
%define version  0.15.1
%define release  %mkrel 1

Summary:       Command-line programmer and debugger for PIC and dsPIC microcontrollers
Name:          %{name}
Version:       %{version}
Release:       %{release}
Source0:       piklab-%{version}.tar.bz2
License:       GPL
Group:         Development/Other
Url:           http://piklab.sourceforge.net/
BuildRoot:     %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires: qt3-devel
BuildRequires: libusb-devel libreadline-devel ncurses-devel

Conflicts:     piklab piklab-prog-qt4
Packager:      hadacek@kde.org
Provides:      piklab-prog piklab-hex

%description
"piklab-prog" is a command-line programmer and debugger for PIC and dsPIC
microcontrollers. It supports GPSim, and several Microchip and direct
programmers.
"piklab-hex" is a command-line utility to manipulate hex files.

%prep
%setup -q -n piklab-%{version}

%build
cd %_builddir/%buildsubdir
qmake piklab-prog.pro
#%make uses -j3 which fails
%__make

%install
rm -rf %{buildroot}
%__mkdir_p %{buildroot}/%{_bindir}
cd %_builddir/%buildsubdir/src/piklab-prog
%__install -s -m 755 piklab-prog %{buildroot}/%{_bindir}/piklab-prog
cd %_builddir/%buildsubdir/src/piklab-hex
%__install -s -m 755 piklab-hex %{buildroot}/%{_bindir}/piklab-hex
%__mkdir_p %{buildroot}/%{_mandir}/man1
cd %_builddir/%buildsubdir/man
%__install -m 755 piklab-prog.1 %{buildroot}/%{_mandir}/man1/piklab-prog.1
%__install -m 755 piklab-hex.1 %{buildroot}/%{_mandir}/man1/piklab-hex.1

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,0755)
%doc README INSTALL COPYING Changelog TODO
%{_bindir}/*
%{_mandir}/man1/*

# --- changelog -----------------------------------------------------
%changelog
* Sun Nov 11 2007 Nicolas Hadacek <hadacek@kde.org> 0.15.1-1mdv2008.0
- fixes
* Sat Oct 20 2007 Nicolas Hadacek <hadacek@kde.org> 0.15.0-1mdv2007.0
- new release
* Fri Aug 17 2007 Nicolas Hadacek <hadacek@kde.org> 0.14.5-1mdv2007.0
- fix
* Sun Aug 5 2007 Nicolas Hadacek <hadacek@kde.org> 0.14.4-1mdv2007.0
- fixes
* Fri Jul 27 2007 Nicolas Hadacek <hadacek@kde.org> 0.14.3-1mdk
- fixes + new features
* Mon Apr 9 2007 Nicolas Hadacek <hadacek@kde.org> 0.14.2-1mdk
- fixes + new features
* Sun Mar 11 2007 Nicolas Hadacek <hadacek@kde.org> 0.14.1-1mdk
- fixes + new feature
* Sun Mar 4 2007 Nicolas Hadacek <hadacek@kde.org> 0.14.0-1mdk
- new version
* Thu Feb 8 2007 Nicolas Hadacek <hadacek@kde.org> 0.13.3-1mdk
- fixes
* Wed Jan 31 2007 Nicolas Hadacek <hadacek@kde.org> 0.13.2-1mdk
- fixes
* Sun Jan 28 2007 Nicolas Hadacek <hadacek@kde.org> 0.13.1-1mdk
- fixes
* Fri Jan 19 2007 Nicolas Hadacek <hadacek@kde.org> 0.13.0-1mdk
- new version
* Mon Oct 16 2006 Nicolas Hadacek <hadacek@kde.org> 0.12.2-1mdk
- fixes
* Sun Oct 15 2006 Nicolas Hadacek <hadacek@kde.org> 0.12.1-1mdk
- fixes
* Sun Oct 1 2006 Nicolas Hadacek <hadacek@kde.org> 0.12.0-1mdk
- new version
* Tue Sep 5 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.2-1mdk
- more fixes
* Wed Aug 23 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.1-1mdk
- some fixes
* Sun Aug 20 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.0-1mdk
- new version
