AC_PREREQ([2.64])
AC_INIT([maynard],
        [0.2.0+git])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz])

AM_SILENT_RULES([yes])

# Check for programs
AC_PROG_CC

IT_PROG_INTLTOOL([0.40])

GETTEXT_PACKAGE=maynard
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
                   [The prefix for our gettext translation domains.])

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG()

PKG_CHECK_MODULES([GTK], [
	wayland-client >= 1.0.2
	weston >= 3
	gtk+-3.0 >= 3.10
	gtk+-wayland-3.0
	libgnome-menu-3.0
	gnome-desktop-3.0
	alsa
])

PKG_CHECK_MODULES([WESTON], [
	weston >= 3
	libweston-3
])

GLIB_GSETTINGS

WAYLAND_SCANNER_RULES(['$(top_srcdir)/protocol'])

AC_CONFIG_FILES([maynard], [chmod +x maynard])
AC_CONFIG_FILES([Makefile
		 data/Makefile
		 shell/Makefile
		 po/Makefile.in
		 protocol/Makefile
		 ])
AC_OUTPUT
