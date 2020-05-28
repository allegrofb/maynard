#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <gdk/gdkkeysyms.h>

#include "Config.h"
#include "Wayland.h"
#include "Background.h"
#include "MaynardPanel.h"
#include "MaynardCurtain.h"
#include "MaynardLauncher.h"
#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"
#include "../shell/maynard-resources.h"

// static void
// button_toggled_cb (struct desktop *desktop,
//     gboolean *visible,
//     gboolean *not_visible)
// {
//   *visible = !*visible;
//   *not_visible = FALSE;

//   if (desktop->system_visible)
//     {
//       maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
//           MAYNARD_CLOCK_SECTION_SYSTEM);
//       maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
//           MAYNARD_PANEL_BUTTON_SYSTEM);
//     }
//   else if (desktop->volume_visible)
//     {
//       maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
//           MAYNARD_CLOCK_SECTION_VOLUME);
//       maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
//           MAYNARD_PANEL_BUTTON_VOLUME);
//     }
//   else
//     {
//       maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
//           MAYNARD_CLOCK_SECTION_CLOCK);
//       maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
//           MAYNARD_PANEL_BUTTON_NONE);
//     }
// }

// static void
// system_toggled_cb (GtkWidget *widget,
//     struct desktop *desktop)
// {
//   button_toggled_cb (desktop,
//       &desktop->system_visible,
//       &desktop->volume_visible);
// }

// static void
// volume_toggled_cb (GtkWidget *widget,
//     struct desktop *desktop)
// {
//   button_toggled_cb (desktop,
//       &desktop->volume_visible,
//       &desktop->system_visible);
// }

static gboolean
panel_window_enter_cb(GtkWidget *widget,
                      GdkEventCrossing *event,
                      Wayland *desktop)
{
    if (desktop->initial_panel_timeout_id > 0)
    {
        g_source_remove(desktop->initial_panel_timeout_id);
        desktop->initial_panel_timeout_id = 0;
    }

    if (desktop->hide_panel_idle_id > 0)
    {
        g_source_remove(desktop->hide_panel_idle_id);
        desktop->hide_panel_idle_id = 0;
        return FALSE;
    }

    if (desktop->pointer_out_of_panel)
    {
        desktop->pointer_out_of_panel = FALSE;
        return FALSE;
    }

    shell_helper_slide_surface_back(desktop->helper,
                                    desktop->panel.surface);
    //   shell_helper_slide_surface_back (desktop->helper,
    //       desktop->clock->surface);

    // maynard_panel_set_expand(MAYNARD_PANEL(desktop->panel->window), TRUE);

    return FALSE;
}

static gboolean
leave_panel_idle_cb(gpointer data)
{
    Wayland *desktop = (Wayland *)data;
    gint width;

    desktop->hide_panel_idle_id = 0;

    // gtk_window_get_size(GTK_WINDOW(desktop->clock.window),
    //                     &width, NULL);

    shell_helper_slide_surface(desktop->helper,
                               desktop->panel.surface,
                               MAYNARD_VERTICAL_CLOCK_WIDTH - MAYNARD_PANEL_WIDTH, 0);
    // shell_helper_slide_surface(desktop->helper,
    //                            desktop->clock.surface,
    //                            MAYNARD_VERTICAL_CLOCK_WIDTH - MAYNARD_PANEL_WIDTH - width, 0);

    // maynard_panel_set_expand(MAYNARD_PANEL(desktop->panel->window), FALSE);

    // maynard_clock_show_section(MAYNARD_CLOCK(desktop->clock->window),
    //                            MAYNARD_CLOCK_SECTION_CLOCK);
    // maynard_panel_show_previous(MAYNARD_PANEL(desktop->panel->window),
    //                             MAYNARD_PANEL_BUTTON_NONE);
    desktop->system_visible = FALSE;
    desktop->volume_visible = FALSE;
    desktop->pointer_out_of_panel = FALSE;

    return G_SOURCE_REMOVE;
}

static gboolean
panel_window_leave_cb(GtkWidget *widget,
                      GdkEventCrossing *event,
                      Wayland *desktop)
{
    if (desktop->initial_panel_timeout_id > 0)
    {
        g_source_remove(desktop->initial_panel_timeout_id);
        desktop->initial_panel_timeout_id = 0;
    }

    if (desktop->hide_panel_idle_id > 0)
        return FALSE;

    if (desktop->grid_visible)
    {
        desktop->pointer_out_of_panel = TRUE;
        return FALSE;
    }

    desktop->hide_panel_idle_id = g_idle_add(leave_panel_idle_cb, desktop);

    return FALSE;
}

static gboolean
panel_hide_timeout_cb (gpointer data)
{
  Wayland *desktop = (Wayland*)data;

  panel_window_leave_cb (NULL, NULL, desktop);

  return G_SOURCE_REMOVE;
}

static void
css_setup()
{
    GtkCssProvider *provider;
    GFile *file;
    GError *error = NULL;

    provider = gtk_css_provider_new();

    file = g_file_new_for_uri("resource:///org/raspberry-pi/maynard/style.css");

    if (!gtk_css_provider_load_from_file(provider, file, &error)) //hyjiang, gtk api load css
    {
        g_warning("Failed to load CSS file: %s", error->message);
        g_clear_error(&error);
        g_object_unref(file);
        return;
    }

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider), 600);

    g_object_unref(file);
}

static gboolean
connect_enter_leave_signals(gpointer data)
{
    Wayland *desktop = (Wayland *)data;
    GList *l;

    g_signal_connect(desktop->panel.window, "enter-notify-event",
                     G_CALLBACK(panel_window_enter_cb), desktop);
    g_signal_connect(desktop->panel.window, "leave-notify-event",
                     G_CALLBACK(panel_window_leave_cb), desktop);

    // g_signal_connect(desktop->clock.window, "enter-notify-event",
    //                  G_CALLBACK(panel_window_enter_cb), desktop);
    // g_signal_connect(desktop->clock.window, "leave-notify-event",
    //                  G_CALLBACK(panel_window_leave_cb), desktop);

    g_signal_connect(desktop->launcher_grid.window, "enter-notify-event",
                     G_CALLBACK(panel_window_enter_cb), desktop);
    g_signal_connect(desktop->launcher_grid.window, "leave-notify-event",
                     G_CALLBACK(panel_window_leave_cb), desktop);

    return G_SOURCE_REMOVE;
}

int main(int argc, char *argv[])
{
    gdk_set_allowed_backends("wayland");
    Gtk::Main kit(argc, argv);

    // auto app =
    //     Gtk::Application::create(argc, argv,
    //                              "org.gtkmm.examples.base");

    Wayland *g = Wayland::getInstance();

    /* Wait until we have been notified about the compositor,
   * shell, and shell helper objects */
    if (!g->output || !g->wshell || !g->helper)
        wl_display_roundtrip(g->display);
    if (!g->output || !g->wshell || !g->helper)
    {
        fprintf(stderr, "could not find output, shell or helper modules\n");
        fprintf(stderr, "output: %p, wshell: %p, helper: %p\n",
                g->output, g->wshell, g->helper);
        return -1;
    }

    css_setup();

    BackgroundWindow backgournd;
    backgournd.show();
    // MaynardCurtain curtain;
    // curtain.show();

    MaynardPanel panel;
    panel.app_menu_toggled.connect(
        sigc::mem_fun(*g, &Wayland::launcher_grid_toggle));
    panel.system_toggled.connect(
        sigc::mem_fun(*g, &Wayland::system_toggled));
    panel.volume_toggled.connect(
        sigc::mem_fun(*g, &Wayland::volume_toggled));
    panel.favorite_launched.connect(
        sigc::mem_fun(*g, &Wayland::favorite_launched));
    g->initial_panel_timeout_id =
        g_timeout_add_seconds(2, panel_hide_timeout_cb, g);
    panel.show();

    // clock_create (desktop);

    MaynardLauncher launcher;
    // g_signal_connect(g->launcher_grid->window, "app-launched",
    //                  G_CALLBACK(launcher_grid_toggle), g);

    launcher.show();

    // grab_surface_create (desktop);

    //     /* TODO: why does the panel signal leave on drawing for
    //    * startup? we don't want to have to have this silly
    //    * timeout. */
    g_timeout_add_seconds(1, connect_enter_leave_signals, g); //hyjiang, setup enter leave signals

    // return app->run(window);
    Gtk::Main::run();
    return EXIT_SUCCESS;
}
