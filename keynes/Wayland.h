#ifndef WAYLAND_H
#define WAYLAND_H

#include <gtkmm.h>
#include <gdk/gdkwayland.h>
#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"

class Wayland;

    struct rectangle
    {
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    };

    struct output
    {
        Wayland *wayland;
        struct wl_output *output;
        uint32_t server_output_id;
        struct rectangle allocation;
        struct wl_list link;
        int transform;
        int scale;
        char *make;
        char *model;

        // display_output_handler_t destroy_handler;
        void *user_data;
    };


// struct output {
// 	struct wl_output *output;
// 	uint32_t server_output_id;
// 	struct wl_list link;

// 	int x;
// 	int y;
// 	struct panel *panel;
// 	struct background *background;
// };

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
    void desktop_ready();

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

        element()
        {
            window = NULL;
            pixbuf = NULL;
            surface = NULL;
        }
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

	struct wl_list outputs;

public:
};

#endif //WAYLAND_H