#ifndef WAYLAND_H
#define WAYLAND_H

#include <gtkmm.h>
#include <gdk/gdkwayland.h>
#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"

class Wayland
{
private:
    /* Here will be the instance stored. */
    static Wayland *instance;

    /* Private constructor to prevent instancing. */
    Wayland();

public:
    /* Static access method. */
    static Wayland *getInstance();

public:
    void launcher_grid_toggle();
    void system_toggled();
    void volume_toggled();
    void favorite_launched();

    void button_toggled_cb(
        gboolean *visible,
        gboolean *not_visible);

public:
    gboolean grid_visible;
    gboolean system_visible;
    gboolean volume_visible;
    gboolean pointer_out_of_panel;

public:
    struct element
    {
        GtkWidget *window;
        GdkPixbuf *pixbuf;
        struct wl_surface *surface;
    };

    struct element background;
    struct element panel;
    struct element curtain;
    struct element launcher_grid;
    struct element clock;

    guint initial_panel_timeout_id;
    guint hide_panel_idle_id;

public:
    struct wl_display *display;
    struct wl_registry *registry;
    struct weston_desktop_shell *wshell;
    struct wl_output *output;
    struct shell_helper *helper;

    GdkDisplay *gdk_display;

    struct wl_seat *seat;
    struct wl_pointer *pointer;

};

#endif //WAYLAND_H