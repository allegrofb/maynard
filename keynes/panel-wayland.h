#ifndef PANEL_WAYLAND_H
#define PANEL_WAYLAND_H

#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>                      
#include "weston-desktop-shell-client-protocol.h"

G_BEGIN_DECLS


void gnome_panel_create(void *g,
                           struct weston_desktop_shell *wshell,
                           struct wl_output *output,
                           GtkWidget **window,
                           GdkPixbuf **pixbuf,
                           struct wl_surface **surface);

// int gnome_panel_wl_create(void);

// void gnome_panel_wl_assign(void *wayland,
//                            struct weston_desktop_shell *wshell,
//                            struct wl_output *output,
//                            GtkWidget **window,
//                            GdkPixbuf **pixbuf,
//                            struct wl_surface **surface);

// gboolean gnome_panel_wl_setup(GtkWidget* widget);

G_END_DECLS

#endif  //PANEL_WAYLAND_H
