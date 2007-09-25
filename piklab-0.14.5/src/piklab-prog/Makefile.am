INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = piklab-prog
OBJ = $(top_builddir)/src/progs/list/libproglistnoui.la \
	$(top_builddir)/src/progs/bootloader/base/libbootloader.la \
    $(top_builddir)/src/progs/bootloader/bootloader_data/libbootloaderdata.la \
	$(top_builddir)/src/progs/gpsim/libgpsim.la $(top_builddir)/src/progs/psp/base/libpsp.la \
	$(top_builddir)/src/progs/pickit1/base/libpickit1.la \
	$(top_builddir)/src/progs/pickit2v2/base/libpickit2v2.la $(top_builddir)/src/progs/pickit2/base/libpickit2.la \
	$(top_builddir)/src/progs/icd1/base/libicd1.la $(top_builddir)/src/progs/icd2/base/libicd2.la \
	$(top_builddir)/src/progs/icd2/icd2_data/libicd2data.la $(top_builddir)/src/progs/direct/base/libdirectprog.la \
	$(top_builddir)/src/progs/icd2/icd2_data/libicd2data.la $(top_builddir)/src/progs/usbpicprog/base/libusbpicprogprog.la \
	$(top_builddir)/src/progs/manager/libprogmanager.la $(top_builddir)/src/progs/base/libprogbase.la \
	$(top_builddir)/src/coff/base/libcoff.la $(top_builddir)/src/common/port/libport.la \
	$(top_builddir)/src/libcli/libcli.la $(top_builddir)/src/common/global/libglobal.la \
    $(top_builddir)/src/devices/list/libdevicelistnoui.la \
	$(top_builddir)/src/devices/pic/pic/libpic.la $(top_builddir)/src/devices/pic/base/libpicbase.la \
	$(top_builddir)/src/devices/pic/xml_data/libpicxml.la \
	$(top_builddir)/src/devices/mem24/mem24/libmem24.la $(top_builddir)/src/devices/mem24/base/libmem24base.la \
	$(top_builddir)/src/devices/mem24/xml_data/libmem24xml.la \
	$(top_builddir)/src/devices/base/libdevicebase.la \
	$(top_builddir)/src/common/common/libcommon.la
piklab_prog_DEPENDENCIES = $(OBJ)
piklab_prog_LDADD = $(OBJ) $(LIB_KDECORE) $(LIBUSB_LIBS) $(LIBREADLINE_LIBS)
piklab_prog_LDFLAGS = $(all_libraries) $(KDE_RPATH)
piklab_prog_SOURCES = cmdline.cpp cli_prog_manager.cpp cli_debug_manager.cpp cli_interactive.cpp
