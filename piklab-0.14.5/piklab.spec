%define name     piklab
%define version  0.14.5
%define release  %mkrel 1
%define unstable 1

Name:           %{name}
Summary:        IDE for PIC and dsPIC microcontrollers
Version:        %{version}
Release:        %{release}
Source0:        http://prdownloads.sourceforge.net/piklab/%{name}-%{version}.tar.bz2
URL:            http://piklab.sourceforge.net
BuildRequires:  qt3-devel kdelibs-devel libart_lgpl-devel fam-devel libusb-devel readline-devel
Group:          Development/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-buildroot
License:        GPL
Packager:       hadacek@kde.org
Icon:           piklab.xpm
Conflicts:      piklab-prog-qt3 piklab-prog-qt4
Provides:       piklab-prog piklab-hex

%description
Piklab is a graphic development environment for PIC and dsPIC microcontrollers.
It interfaces with various toochains for compiling and assembling and it
supports several Microchip and direct programmers.

# --- prep section --------------------------------------------------
%prep
%setup -q

# --- build section -------------------------------------------------
%build
export QTDIR=%_prefix/lib/qt3
export KDEDIR=%_prefix
export LD_LIBRARY_PATH=$QTDIR/%{_lib}:$KDEDIR/%{_lib}:$LD_LIBRARY_PATH
export PATH=$QTDIR/bin:$KDEDIR/bin:$PATH
export QTLIB=$QTDIR/%{_lib}

cd %_builddir/%buildsubdir
%__make -f admin/Makefile.common
CFLAGS="%optflags" CXXFLAGS="%optflags" \
%configure --disable-rpath --datadir=%_datadir \
%if %unstable
           --enable-debug=full
%else
           --disable-debug
%endif

%make

# --- install section -----------------------------------------------
%install
%__rm -rf %buildroot
%makeinstall
cd -

desktop-file-install --vendor="" --add-category="X-MandrivaLinux-MoreApplications-Development-DevelopmentEnvironments" --dir %{buildroot}%{_datadir}/applications/kde %{buildroot}%{_datadir}/applications/kde/piklab.desktop

#icons for rpmlint
%__mkdir_p %buildroot/{%_liconsdir,%_miconsdir,%_iconsdir}
%__ln_s %_datadir/icons/hicolor/64x64/apps/piklab.png %buildroot/%_liconsdir
%__ln_s %_datadir/icons/hicolor/32x32/apps/piklab.png %buildroot/%_iconsdir
%__ln_s %_datadir/icons/hicolor/16x16/apps/piklab.png %buildroot/%_miconsdir
cd -

%{find_lang} piklab

# --- post-install section ------------------------------------------
%post
%{update_menus}
%update_desktop_database
%update_mime_database
%update_icon_cache hicolor
%update_icon_cache crystalsvg

# --- post-uninstall section ----------------------------------------
%postun
%{clean_menus}
%clean_desktop_database
%clean_mime_database
%update_icon_cache hicolor
%update_icon_cache crystalsvg

# --- clean section -------------------------------------------------
%clean
%__rm -rf %buildroot

# --- files section -------------------------------------------------
%files -f piklab.lang
%defattr(-,root,root,0755)
%doc README INSTALL COPYING Changelog TODO
%_bindir/piklab
%_bindir/piklab-prog
%_bindir/piklab-hex
%dir %_datadir/apps/piklab/
%_datadir/apps/piklab/*
%_iconsdir/hicolor/*
%_liconsdir/piklab.png
%_iconsdir/piklab.png
%_miconsdir/piklab.png
%dir %_datadir/apps/katepart/syntax/
%_datadir/apps/katepart/syntax/asm-pic.xml
%_datadir/apps/katepart/syntax/coff-pic.xml
%_datadir/apps/katepart/syntax/coff-c-pic.xml
%_datadir/apps/katepart/syntax/jal-pic.xml
%_datadir/applications/kde/piklab.desktop
%_datadir/mimelnk/application/x-piklab.desktop
%dir %_datadir/doc/HTML/en/piklab
%_datadir/doc/HTML/en/piklab/index.docbook
%_datadir/doc/HTML/en/piklab/index.cache.bz2
%_datadir/doc/HTML/en/piklab/common
%_mandir/man1/*

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
