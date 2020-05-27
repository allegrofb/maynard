#include <iostream>
#include "WlWindow.h"
#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"

#include "Wayland.h"


WlWindow::WlWindow()
{
    set_resizable(false);
    set_decorated(false);

    this->window = (GtkWidget *)gobj();

    realize();

    GdkWindow *gdk_window;
    gdk_window = gtk_widget_get_window(this->window);
    gdk_wayland_window_set_use_custom_surface(gdk_window);

    this->surface = gdk_wayland_window_get_wl_surface(gdk_window);

    weston_desktop_shell_set_user_data(Wayland::getInstance()->wshell, Wayland::getInstance());
}

WlWindow::~WlWindow()
{
}
