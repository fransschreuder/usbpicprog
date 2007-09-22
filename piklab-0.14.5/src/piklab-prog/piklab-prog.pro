STOPDIR = ../..
include($${STOPDIR}/app.pro)

TARGET   = piklab-prog
HEADERS += cli_prog_manager.h cli_debug_manager.h cli_interactive.h cmdline.h
SOURCES += cli_prog_manager.cpp cli_debug_manager.cpp cli_interactive.cpp cmdline.cpp
LIBS    += ../libcli/libcli.a ../progs/manager/libprogmanager.a ../progs/list/libprogslist.a \
           ../progs/bootloader/base/libbootloader.a ../progs/bootloader/bootloader_data/libbootloader_data.a \
           ../progs/gpsim/libgpsim.a \
           ../progs/psp/base/libpsp.a ../progs/pickit1/base/libpickit1.a \
           ../progs/pickit2/base/libpickit2.a ../progs/icd1/base/libicd1.a \
           ../progs/icd2/base/libicd2.a \
           ../progs/icd2/icd2_data/libicd2data.a ../progs/direct/base/libdirectprog.a \
           ../progs/base/libprogbase.a ../common/port/libport.a ../coff/base/libcoff.a \
           ../devices/list/libdevicelist.a \
           ../devices/mem24/mem24/libmem24.a ../devices/mem24/xml_data/libmem24xml.a \
           ../devices/mem24/base/libmem24base.a \
           ../devices/pic/pic/libpic.a ../devices/pic/xml_data/libpicxml.a \
           ../devices/pic/base/libpicbase.a ../xml_to_data/libxmltodata.a \
           ../devices/base/libdevicebase.a ../common/global/libglobal.a \
           ../common/nokde/libnokde.a ../common/common/libcommon.a
contains(DEFINES, HAVE_USB) {
  unix:LIBS += -lusb
  win32:LIBS += $${LIBUSB_PATH}\lib\gcc\libusb.a
}
contains(DEFINES, HAVE_READLINE) {
  unix:LIBS += -lhistory -lreadline -lcurses
}
