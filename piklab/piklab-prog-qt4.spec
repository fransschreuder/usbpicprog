%define name     piklab-prog-qt4
%define version  0.14.5
%define release  %mkrel 1
%define unstable 1

Name:           %{name}
Summary:        Command-line programmer and debugger for PIC and dsPIC microcontrollers
Version:        %{version}
Release:        %{release}
Source0:        http://prdownloads.sourceforge.net/piklab/piklab-%{version}.tar.bz2
URL:            http://piklab.sourceforge.net
BuildRequires:  qt4-devel libusb-devel libreadline-devel
Group:          Development/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-buildroot
License:        GPL
Packager:       hadacek@kde.org
Icon:           piklab.xpm
Conflicts:      piklab piklab-prog-qt3
Provides:       piklab-prog piklab-hex

%description
piklab-prog is a command-line programmer and debugger for PIC and dsPIC
microcontrollers. It supports GPSim, and several Microchip and direct
programmers.

# --- prep section --------------------------------------------------

%prep
%setup -q -n piklab-%{version}

# --- build section -------------------------------------------------
%build
export QTDIR=%_prefix/%{_lib}/qt4
export LD_LIBRARY_PATH=$QTDIR/%{_lib}:$LD_LIBRARY_PATH
export PATH=$QTDIR/bin:$PATH
export QTLIB=$QTDIR/%{_lib}
cd %_builddir/%buildsubdir
qmake piklab-prog.pro
%__make

# --- install section -----------------------------------------------
%install
%__rm -rf %buildroot

%__mkdir_p %buildroot/%_bindir
cd %_builddir/%buildsubdir/src/piklab-prog
%__install -s piklab-prog %buildroot/%_bindir/piklab-prog
cd %_builddir/%buildsubdir/src/piklab-hex
%__install -s piklab-hex %buildroot/%_bindir/piklab-hex

%__mkdir_p %buildroot/%_mandir
cd %_builddir/%buildsubdir/man
%__install piklab-prog.1 %buildroot/%_mandir/piklab-prog.1
%__install piklab-hex.1 %buildroot/%_mandir/piklab-hex.1

# --- clean section -------------------------------------------------
%clean
%__rm -rf %buildroot

# --- files section -------------------------------------------------
%files
%defattr(-,root,root,0755)
%doc README INSTALL COPYING Changelog TODO
%_bindir/*
%_mandir/*

# --- changelog -----------------------------------------------------
%changelog
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
