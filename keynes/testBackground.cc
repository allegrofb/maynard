#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkwayland.h>
#include <gdk/gdkkeysyms.h>

#include "Config.h"
#include "Wayland.h"
#include "Background.h"
#include "panel-wayland.h"

gboolean test(void *data)
{
    Wayland *g = (Wayland*)data;
    // printf("%s[%d] gnome_panel_wl_assign\n",__FILE__,__LINE__);
    // gnome_panel_wl_assign(g,
    //                       g->wshell,
    //                       g->output,
    //                       &(g->panel.window),
    //                       &(g->panel.pixbuf),
    //                       &(g->panel.surface));
    // printf("%s[%d] gnome_panel_wl_create\n",__FILE__,__LINE__);
    // gnome_panel_wl_create();

    printf("%s[%d] desktop_ready\n",__FILE__,__LINE__);
    g->desktop_ready();

}

int main(int argc, char *argv[])
{
    gdk_set_allowed_backends("wayland");
    Gtk::Main kit(argc, argv);
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

    printf("%s[%d] background\n",__FILE__,__LINE__);
    BackgroundWindow backgournd;
    backgournd.show();

    gnome_panel_create(g,
                       g->wshell,
                       g->output,
                       &(g->panel.window),
                       &(g->panel.pixbuf),
                       &(g->panel.surface));

    // printf("%s[%d] gnome_panel_wl_assign\n",__FILE__,__LINE__);
    // gnome_panel_wl_assign(g,
    //                       g->wshell,
    //                       g->output,
    //                       &(g->panel.window),
    //                       &(g->panel.pixbuf),
    //                       &(g->panel.surface));
    // printf("%s[%d] gnome_panel_wl_create\n",__FILE__,__LINE__);
    // gnome_panel_wl_create();


    // g_timeout_add_seconds (2, test, g);


    printf("%s[%d] desktop_ready\n",__FILE__,__LINE__);
    g->desktop_ready();

    printf("%s[%d] Gtk::Main::run()\n",__FILE__,__LINE__);
    Gtk::Main::run();

    return EXIT_SUCCESS;
}
