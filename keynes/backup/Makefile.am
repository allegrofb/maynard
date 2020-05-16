libexec_PROGRAMS = keynes

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(CLIENT_CFLAGS) $(CORE_CFLAGS)
AM_CXXFLAGS = $(CLIENT_CFLAGS) $(GTK_CFLAGS) $(CORE_CFLAGS) $(IMAGE_CFLAGS) 

noinst_LTLIBRARIES = \
	libttt.la

libttt_la_SOURCES = \
	yapp.cc \
	yxapp.cc \
	ytime.cc \
	ytimer.cc \
	ywindow.cc \
	ycolor.cc \
	ypaint.cc \
	ypopup.cc \
	misc.cc \
	ycursor.cc \
	ypaths.cc \
	ylocale.cc \
	yarray.cc \
	ycollections.cc \
	ypipereader.cc \
	yxembed.cc \
	yconfig.cc \
	yprefs.cc \
	yfont.cc \
	yfontcore.cc \
	yfontxft.cc \
	ypixmap.cc \
	yimage_gdk.cc \
	yximage.cc \
	udir.cc \
	upath.cc \
	mstring.cc \
	ytooltip.cc \
	ref.cc \
	ymenu.cc \
	ymenuitem.cc \
	yicon.cc \
	wmconfig.cc \
	wpixres.cc 

libttt_la_SOURCES += \
	widget.c \
	acpustatus.cc \
	applet.cc


keynes_SOURCES = \
	keynes.cc \
	../shell/weston-desktop-shell-client-protocol.h	\
	../shell/weston-desktop-shell-protocol.c		\
	../shell/shell-helper-client-protocol.h		\
	../shell/shell-helper-protocol.c

keynes_LDADD = $(GTK_LIBS) -lm



