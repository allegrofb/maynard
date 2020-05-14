AC_PREREQ([2.64])
AC_INIT([maynard],
        [0.2.0+git])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 foreign no-dist-gzip dist-xz])

AM_SILENT_RULES([yes])

# Check for programs
AC_LANG_CPLUSPLUS
AC_PROG_SED
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_ARG_VAR([LD], [Linker loader command])

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
	xkbcommon >= 0.3.0
])

PKG_CHECK_MODULES([WESTON], [
	weston >= 3
	libweston-3
])

PKG_CHECK_MODULES([CORE],[fontconfig xrender xcomposite xdamage xfixes xext x11 gtk+-3.0])
AC_SUBST([CORE_CFLAGS])
AC_SUBST([CORE_LIBS])

AC_ARG_ENABLE([gdk-pixbuf],
    AC_HELP_STRING([--disable-gdk-pixbuf],[Disable gdk-pixbuf rendering.
	@<:@default=enabled@:>@]))
if test "x$enable_gdk_pixbuf" != xno ; then
    PKG_CHECK_MODULES([PIXBUF],[gdk-pixbuf-xlib-2.0],[
	IMAGE_CFLAGS="$PIXBUF_CFLAGS"
	IMAGE_LIBS="$PIXBUF_LIBS"
	AC_DEFINE([CONFIG_GDK_PIXBUF_XLIB],[1],[Define to enable gdk-pixbuf rendering.])
	image_library=gdk-pixbuf-xlib-2.0])
    PKG_CHECK_MODULES([LIBRSVG],[librsvg-2.0],[
	IMAGE_CFLAGS="$LIBRSVG_CFLAGS $IMAGE_CFLAGS"
	IMAG_LIBS="$LIBRSVG_LIBS $IMAGE_LIBS"
	AC_DEFINE([CONFIG_LIBRSVG],[1],[Define to enable librsvg rendering.])
	image_library="$image_library librsvg-2.0"],
	[AC_MSG_WARN([SVG is not supported.])])
    PKG_CHECK_MODULES([XPM],[xpm],[
	IMAGE_CFLAGS="$IMAGE_CFLAGS $XPM_CFLAGS"
	IMAGE_LIBS="$IMAGE_LIBS $XPM_LIBS"
	AC_DEFINE([CONFIG_XPM],[1],[Define to enable libxpm rendering.])
	image_library="$image_library libxpm"])
else
    PKG_CHECK_MODULES([XPM],[xpm],[
	IMAGE_CFLAGS="$XPM_CFLAGS"
	IMAGE_LIBS="$XPM_LIBS"
	AC_DEFINE([CONFIG_XPM],[1],[Define to enable libxpm rendering.])
	image_library=libxpm])
    PKG_CHECK_MODULES([LIBPNG],[libpng >= 1.2],[
	IMAGE_CFLAGS="$LIBPNG_CFLAGS $IMAGE_CFLAGS"
	IMAGE_LIBS="$LIBPNG_LIBS $IMAGE_LIBS"
	AC_DEFINE([CONFIG_LIBPNG],[1],[Define to enable libpng rendering.])
	image_library="$image_library libpng"])
    PKG_CHECK_MODULES([LIBJPEG],[libjpeg],[
        IMAGE_CFLAGS="$LIBJPEG_CFLAGS $IMAGE_CFLAGS"
        IMAGE_LIBS="$LIBJPEG_LIBS $IMAGE_LIBS"
        AC_DEFINE([CONFIG_LIBJPEG],[1],[Define to enable libjpeg rendering.])
        image_library="$image_library libjpeg"],
        [if test -f /usr/include/jpeglib.h &&
            test -x /usr/lib64/libjpeg.so ; then
            IMAGE_LIBS="-ljpeg $IMAGE_LIBS"
            AC_DEFINE([CONFIG_LIBJPEG],[1],[Define to enable libjpeg rendering.])
        fi])
fi
AC_SUBST([IMAGE_CFLAGS])
AC_SUBST([IMAGE_LIBS])

GLIB_GSETTINGS

WAYLAND_SCANNER_RULES(['$(top_srcdir)/protocol'])

AC_CONFIG_FILES([maynard], [chmod +x maynard])
AC_CONFIG_FILES([Makefile
		 data/Makefile
		 keynes/Makefile
		 shell/Makefile
		 po/Makefile.in
		 protocol/Makefile
		 ])
AC_OUTPUT
