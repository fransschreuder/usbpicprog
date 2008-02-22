%define name piklab
%define version 0.15.1
%define release %mkrel 1

Summary: IDE for applications based on PIC microcontrollers
Name: %{name}
Version: %{version}
Release: %{release}
Source0: %{name}-%{version}.tar.bz2
License: GPL
Group: Development/Other
Url: http://piklab.sourceforge.net/
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildRequires: kdelibs-devel
#BuildRequires: kdebase-devel
BuildRequires: libusb-devel libreadline-devel ncurses-devel

Packager:       hadacek@kde.org
Conflicts:      piklab-prog-qt3 piklab-prog-qt4

%description
Piklab is an integrated development environment for applications based on
PIC microcontrollers. Gputils tools are used for assembling. Microchip
programmers and several direct programmers are supported.

%prep
%setup -q -n %{name}-%{version}

%build
%configure
# --disable-rpath --with-qt-libraries=$QTDIR/%{_lib}
%make

%install
rm -rf %{buildroot}
%makeinstall

mkdir -p %{buildroot}%{_iconsdir} %{buildroot}%{_miconsdir} %{buildroot}%{_liconsdir}
convert -resize 16x16 piklab.xpm %{buildroot}%{_miconsdir}/%name.png
convert -resize 32x32 piklab.xpm %{buildroot}%{_iconsdir}/%name.png
convert -resize 48x48 piklab.xpm %{buildroot}%{_liconsdir}/%name.png

%find_lang %{name}
%clean
rm -rf %{buildroot}

%post
%{update_menus}

%postun
%{clean_menus}

%files -f %{name}.lang
%defattr(755,root,root)
%{_bindir}/*
%defattr(644,root,root,755)
%{_datadir}/apps/%{name}
%{_datadir}/applications/kde/piklab.desktop
%{_datadir}/icons/hicolor/*/*/*.png
%{_datadir}/mimelnk/application/x-piklab.desktop
%{_datadir}/apps/katepart/syntax/*
%{_mandir}/man1/*
%_docdir/HTML/en/%name
%{_miconsdir}/%name.png
%{_iconsdir}/%name.png
%{_liconsdir}/%name.png

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
* Tue Sep 7 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.2-2mdk
- fix linking of piklab-prog
* Tue Sep 5 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.2-1mdk
- more fixes
* Wed Aug 23 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.1-1mdk
- some fixes
* Sun Aug 20 2006 Nicolas Hadacek <hadacek@kde.org> 0.11.0-1mdk
- new version
* Thu Jul 6 2006 Nicolas Hadacek <hadacek@kde.org> 0.10.0-1mdk
- new version
* Mon May 29 2006 Nicolas Hadacek <hadacek@kde.org> 0.9.0-1mdk
- new version
* Sat May 6 2006 Nicolas Hadacek <hadacek@kde.org> 0.8.0-1mdk
- new version
* Sat Apr 22 2006 Nicolas Hadacek <hadacek@kde.org> 0.7.0-1mdk
- new version
* Wed Mar 23 2006 Nicolas Hadacek <hadacek@kde.org> 0.6.1-1mdk
- fixes
* Wed Mar 15 2006 Nicolas Hadacek <hadacek@kde.org> 0.6.0-1mdk
- new version
* Wed Mar 1 2006 Nicolas Hadacek <hadacek@kde.org> 0.5.2-1mdk
- some fixes
* Tue Feb 28 2006 Nicolas Hadacek <hadacek@kde.org> 0.5.1-1mdk
- some fixes
* Sun Feb 26 2006 Nicolas Hadacek <hadacek@kde.org> 0.5.0-1mdk
- new version
* Sat Feb 18 2006 Nicolas Hadacek <hadacek@kde.org> 0.4.2-1mdk
- some fixes
* Tue Feb 14 2006 Nicolas Hadacek <hadacek@kde.org> 0.4.1-1mdk
- some fixes
* Sun Feb 12 2006 Nicolas Hadacek <hadacek@kde.org> 0.4-1mdk
- new version
* Sun Jan 22 2006 Nicolas Hadacek <hadacek@kde.org> 0.3.1-1mdk
- freeze fix for serial ICD2
* Sat Jan 21 2006 Nicolas Hadacek <hadacek@kde.org> 0.3-1mdk
- new version
* Tue Dec 12 2005 Nicolas Hadacek <hadacek@kde.org> 0.2-1mdk
- new version
* Thu Jun 29 2005 Nicolas Hadacek <hadacek@kde.org> 0.1-1mdk
- initial release
