
#include "Wayland.h"
#include "Background.h"
#include "Panel.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <gdk/gdkkeysyms.h>

#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"

#include "../shell/maynard-resources.h"

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

    PanelWindow panel;
    panel.show();

    // return app->run(window);
    Gtk::Main::run();
    return EXIT_SUCCESS;
}
