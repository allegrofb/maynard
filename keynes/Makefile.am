libexec_PROGRAMS = keynes \
					testMaynardFavorites \
					testMaynardLauncher \
					testMaynardVerticalClock \
					testMaynardPanel

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
	WlWindow.h \
	WlWindow.cc \
	MaynardFavorites.h \
	MaynardFavorites.cc \
	MaynardVerticalClock.h \
	MaynardVerticalClock.cc \
	MaynardPanel.h \
	MaynardPanel.cc \
	MaynardCurtain.h \
	MaynardCurtain.cc \
	MaynardLauncher.h \
	MaynardLauncher.cc \
	Wayland.h \
	Wayland.cc \
	Background.h \
	Background.cc \
	keynes.cc \
	../shell/app-icon.c \
	../shell/app-icon.h \
	../shell/shell-app-system.h \
	../shell/shell-app-system.c \
	../shell/maynard-resources.c \
	../shell/maynard-resources.h \
	../shell/weston-desktop-shell-client-protocol.h	\
	../shell/weston-desktop-shell-protocol.c		\
	../shell/shell-helper-client-protocol.h		\
	../shell/shell-helper-protocol.c

keynes_LDADD = $(GTK_LIBS) -lm


testMaynardFavorites_SOURCES = \
	../shell/app-icon.c \
	../shell/app-icon.h \
	MaynardFavorites.h \
	MaynardFavorites.cc \
	testMaynardFavorites.cc 

testMaynardFavorites_LDADD = $(GTK_LIBS) -lm


testMaynardLauncher_SOURCES = \
	MaynardLauncher.h \
	MaynardLauncher.cc \
	../shell/shell-app-system.c \
	testMaynardLauncher.cc 

testMaynardLauncher_CXXFLAGS = $(AM_CXXFLAGS) -DTEST_APP
testMaynardLauncher_LDADD = $(GTK_LIBS) -lm


testMaynardVerticalClock_SOURCES = \
	MaynardVerticalClock.h \
	MaynardVerticalClock.cc \
	testMaynardVerticalClock.cc 

testMaynardVerticalClock_LDADD = $(GTK_LIBS) -lm

testMaynardPanel_SOURCES = \
	../shell/app-icon.c \
	../shell/app-icon.h \
	MaynardFavorites.h \
	MaynardFavorites.cc \
	MaynardVerticalClock.h \
	MaynardVerticalClock.cc \
	MaynardPanel.h \
	MaynardPanel.cc \
	../shell/shell-app-system.c \
	testMaynardPanel.cc 

testMaynardPanel_CXXFLAGS = $(AM_CXXFLAGS) -DTEST_APP
testMaynardPanel_LDADD = $(GTK_LIBS) -lm

