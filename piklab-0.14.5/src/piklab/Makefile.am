INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = piklab
OBJS = $(top_builddir)/src/libgui/libgui.la $(top_builddir)/src/tools/list/libtoollist.la \
	$(top_builddir)/src/tools/cc5x/libcc5x.la \
	$(top_builddir)/src/tools/mpc/libmpc.la $(top_builddir)/src/tools/boost/libboost.la \
	$(top_builddir)/src/tools/jalv2/libjalv2.la $(top_builddir)/src/tools/ccsc/libccsc.la \
	$(top_builddir)/src/tools/c18/libc18.la $(top_builddir)/src/tools/jal/libjal.la \
	$(top_builddir)/src/tools/pic30/libpic30.la $(top_builddir)/src/tools/picc/libpicc.la \
	$(top_builddir)/src/tools/sdcc/libsdcc.la $(top_builddir)/src/tools/gputils/libgputils.la \
	$(top_builddir)/src/tools/base/libtoolbase.la $(top_builddir)/src/progs/list/libproglistui.la \
	$(top_builddir)/src/progs/bootloader/gui/libbootloaderui.la \
	$(top_builddir)/src/progs/gpsim/gui/libgpsimui.la $(top_builddir)/src/progs/psp/gui/libpspui.la \
	$(top_builddir)/src/progs/pickit2v2/gui/libpickit2v2ui.la \
	$(top_builddir)/src/progs/pickit2/gui/libpickit2ui.la $(top_builddir)/src/progs/pickit1/gui/libpickit1ui.la \
	$(top_builddir)/src/progs/icd1/gui/libicd1ui.la $(top_builddir)/src/progs/icd2/gui/libicd2ui.la \
	$(top_builddir)/src/progs/direct/gui/libdirectui.la $(top_builddir)/src/progs/gui/libprogui.la \
	$(top_builddir)/src/progs/bootloader/base/libbootloader.la \
	$(top_builddir)/src/progs/bootloader/bootloader_data/libbootloaderdata.la \
	$(top_builddir)/src/progs/gpsim/libgpsim.la $(top_builddir)/src/progs/psp/base/libpsp.la \
	$(top_builddir)/src/progs/pickit2v2/base/libpickit2v2.la \
	$(top_builddir)/src/progs/pickit1/base/libpickit1.la $(top_builddir)/src/progs/pickit2/base/libpickit2.la \
	$(top_builddir)/src/progs/icd1/base/libicd1.la $(top_builddir)/src/progs/icd2/base/libicd2.la \
	$(top_builddir)/src/progs/icd2/icd2_data/libicd2data.la $(top_builddir)/src/progs/direct/base/libdirectprog.la \
	$(top_builddir)/src/progs/manager/libprogmanager.la $(top_builddir)/src/progs/base/libprogbase.la \
	$(top_builddir)/src/coff/base/libcoff.la $(top_builddir)/src/common/port/libport.la \
	$(top_builddir)/src/common/global/libglobal.la $(top_builddir)/src/devices/list/libdevicelistui.la \
	$(top_builddir)/src/devices/mem24/gui/libmem24ui.la $(top_builddir)/src/devices/pic/gui/libpicui.la \
	$(top_builddir)/src/devices/gui/libdeviceui.la $(top_builddir)/src/common/gui/libcommonui.la \
	$(top_builddir)/src/devices/pic/pic/libpic.la $(top_builddir)/src/devices/pic/base/libpicbase.la \
	$(top_builddir)/src/devices/pic/xml_data/libpicxml.la \
	$(top_builddir)/src/devices/mem24/mem24/libmem24.la $(top_builddir)/src/devices/mem24/base/libmem24base.la \
	$(top_builddir)/src/devices/mem24/xml_data/libmem24xml.la \
	$(top_builddir)/src/devices/base/libdevicebase.la $(top_builddir)/src/common/common/libcommon.la
piklab_LDADD = $(OBJS) -lktexteditor $(LIBUSB_LIBS) $(LIB_KHTML)
piklab_DEPENDENCIES = $(OBJS)
piklab_LDFLAGS = $(all_libraries) $(KDE_RPATH)
piklab_SOURCES = main.cpp
