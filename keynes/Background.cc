#include <iostream>
#include "Background.h"
#include "../shell/weston-desktop-shell-client-protocol.h"

#include "Wayland.h"

static GdkPixbuf *
scale_background(GdkPixbuf *original_pixbuf)
{
    /* Scale original_pixbuf so it mostly fits on the screen.
   * If the aspect ratio is different than a bit on the right or on the
   * bottom could be cropped out. */
    GdkDisplay *display = gdk_display_get_default();
    /* There's no primary monitor on nested wayland so just use the
     first one for now */
    GdkMonitor *monitor = gdk_display_get_monitor(display, 0);
    GdkRectangle geom;
    gint original_width, original_height;
    gint final_width, final_height;
    gdouble ratio_horizontal, ratio_vertical, ratio;

    g_return_val_if_fail(monitor, NULL);

    gdk_monitor_get_geometry(monitor, &geom);

    original_width = gdk_pixbuf_get_width(original_pixbuf);
    original_height = gdk_pixbuf_get_height(original_pixbuf);

    ratio_horizontal = (double)geom.width / original_width;
    ratio_vertical = (double)geom.height / original_height;
    ratio = MAX(ratio_horizontal, ratio_vertical);

    final_width = ceil(ratio * original_width);
    final_height = ceil(ratio * original_height);

    return gdk_pixbuf_scale_simple(original_pixbuf,
                                   final_width, final_height, GDK_INTERP_BILINEAR);
}

static void load_pixbuf(GdkPixbuf** pixbuf)
{
    const gchar *filename;
    GdkPixbuf *unscaled_background;
    const gchar *xpm_data[] = {"1 1 1 1", "_ c SteelBlue", "_"};

    filename = g_getenv("MAYNARD_BACKGROUND");
    if (filename && filename[0] != '\0')
        unscaled_background = gdk_pixbuf_new_from_file(filename, NULL);
    else
        unscaled_background = gdk_pixbuf_new_from_xpm_data(xpm_data);

    if (!unscaled_background)
    {
        g_message("Could not load background (%s).",
                  filename ? filename : "built-in");
        exit(EXIT_FAILURE);
    }

    *pixbuf = scale_background(unscaled_background);
    g_object_unref(unscaled_background);
}

/* Expose callback for the drawing area */
static gboolean
draw_cb(GtkWidget *widget,
        cairo_t *cr,
        gpointer data)
{
    BackgroundWindow *desktop = (BackgroundWindow *)data;

    gdk_cairo_set_source_pixbuf(cr, desktop->pixbuf, 0, 0);
    cairo_paint(cr);

    return TRUE;
}

BackgroundWindow::BackgroundWindow()
{
    set_title("keynes");
    set_position(Gtk::WIN_POS_CENTER);

    this->signal_draw().connect(sigc::mem_fun(*this, &BackgroundWindow::on_draw));

    Wayland::getInstance()->background.window = this->window;
    Wayland::getInstance()->background.surface = this->surface;
    Wayland::getInstance()->background.pixbuf = this->pixbuf;
    weston_desktop_shell_set_background(Wayland::getInstance()->wshell, Wayland::getInstance()->output,
                                        this->surface);
}

BackgroundWindow::~BackgroundWindow()
{
}

bool BackgroundWindow::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    // Gdk::RGBA color;
    // color.set_red(0.0);
    // color.set_green(0.0);
    // color.set_blue(1.0);
    // color.set_alpha(1.0); //opaque

    Gdk::RGBA color("SteelBlue");

    Gdk::Cairo::set_source_rgba(cr, color);

    //   void set_source(const RefPtr<Surface>& surface, double x, double y);

    // Gdk::Cairo::set_source_pixbuf(pixbuf, 0,0 );

    // gdk_cairo_set_source_pixbuf(cr, desktop->background->pixbuf, 0, 0);

    cr->paint();

    return true;
}
