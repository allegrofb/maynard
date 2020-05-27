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
    struct wl_display *display;
    struct wl_registry *registry;
    struct weston_desktop_shell *wshell;
    struct wl_output *output;
    struct shell_helper *helper;

    GdkDisplay *gdk_display;

    struct wl_seat *seat;
    struct wl_pointer *pointer;

    GtkWidget *background;
    GtkWidget *panel;
};

#endif //WAYLAND_H