
#include "keynes.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>                          
#include <gdk/gdkwayland.h>                      
#include <gdk/gdkkeysyms.h>

#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"

#include "../shell/maynard-resources.h" 



static gboolean
connect_enter_leave_signals (gpointer data)
{
  Global *desktop = (Global*)data;
  GList *l;

//   g_signal_connect (desktop->panel->window, "enter-notify-event",
//       G_CALLBACK (panel_window_enter_cb), desktop);
//   g_signal_connect (desktop->panel->window, "leave-notify-event",
//       G_CALLBACK (panel_window_leave_cb), desktop);

//   g_signal_connect (desktop->clock->window, "enter-notify-event",
//       G_CALLBACK (panel_window_enter_cb), desktop);
//   g_signal_connect (desktop->clock->window, "leave-notify-event",
//       G_CALLBACK (panel_window_leave_cb), desktop);

//   g_signal_connect (desktop->launcher_grid->window, "enter-notify-event",
//       G_CALLBACK (panel_window_enter_cb), desktop);
//   g_signal_connect (desktop->launcher_grid->window, "leave-notify-event",
//       G_CALLBACK (panel_window_leave_cb), desktop);

  return G_SOURCE_REMOVE;
}

static void
shell_configure (Global *desktop,     
    uint32_t edges,
    struct wl_surface *surface,
    int32_t width, int32_t height)
{
  int window_height;
  int grid_width, grid_height;

  gtk_widget_set_size_request (desktop->background->window,    //hyjiang, adjust background size
      width, height);

  /* TODO: make this height a little nicer */
//   window_height = height * MAYNARD_PANEL_HEIGHT_RATIO;
//   gtk_window_resize (GTK_WINDOW (desktop->panel->window),      //hyjiang, adjust panel size
//       MAYNARD_PANEL_WIDTH, window_height);

//   maynard_launcher_calculate (MAYNARD_LAUNCHER (desktop->launcher_grid->window),
//       &grid_width, &grid_height, NULL);
//   gtk_widget_set_size_request (desktop->launcher_grid->window,       //hyjiang, adjust lancher_grid size
//       grid_width, grid_height);

//   shell_helper_move_surface (desktop->helper, desktop->panel->surface,
//       0, (height - window_height) / 2);

//   gtk_window_resize (GTK_WINDOW (desktop->clock->window),             //hyjiang, adjust clock size
//       MAYNARD_CLOCK_WIDTH, MAYNARD_CLOCK_HEIGHT);

//   shell_helper_move_surface (desktop->helper, desktop->clock->surface,
//       MAYNARD_PANEL_WIDTH, (height - window_height) / 2);

//   shell_helper_move_surface (desktop->helper,
//       desktop->launcher_grid->surface,
//       - grid_width,
//       ((height - window_height) / 2) + MAYNARD_CLOCK_HEIGHT);

  weston_desktop_shell_desktop_ready (desktop->wshell);

  /* TODO: why does the panel signal leave on drawing for
   * startup? we don't want to have to have this silly
   * timeout. */
  g_timeout_add_seconds (1, connect_enter_leave_signals, desktop);    //hyjiang, setup enter leave signals
}

static void
weston_desktop_shell_configure (void *data,
    struct weston_desktop_shell *weston_desktop_shell,
    uint32_t edges,
    struct wl_surface *surface,
    int32_t width, int32_t height)
{
  shell_configure((Global*)data, edges, surface, width, height);   
}

static void
weston_desktop_shell_prepare_lock_surface (void *data,
    struct weston_desktop_shell *weston_desktop_shell)
{
  weston_desktop_shell_unlock (weston_desktop_shell);
}

static void
weston_desktop_shell_grab_cursor (void *data,
    struct weston_desktop_shell *weston_desktop_shell,
    uint32_t cursor)
{
}

static const struct weston_desktop_shell_listener wshell_listener = {
  weston_desktop_shell_configure,
  weston_desktop_shell_prepare_lock_surface,
  weston_desktop_shell_grab_cursor
};


static void
pointer_handle_enter (void *data,
    struct wl_pointer *pointer,
    uint32_t serial,
    struct wl_surface *surface,
    wl_fixed_t sx_w,
    wl_fixed_t sy_w)
{
  // fprintf(stderr, "pointer_handle_enter\n");
}

static void
pointer_handle_leave (void *data,
    struct wl_pointer *pointer,
    uint32_t serial,
    struct wl_surface *surface)
{
  // fprintf(stderr, "pointer_handle_leave\n");
}

static void
pointer_handle_motion (void *data,
    struct wl_pointer *pointer,
    uint32_t time,
    wl_fixed_t sx_w,
    wl_fixed_t sy_w)
{
  // fprintf(stderr, "pointer_handle_motion\n");
}

static void
pointer_handle_button (void *data,
    struct wl_pointer *pointer,
    uint32_t serial,
    uint32_t time,
    uint32_t button,
    uint32_t state_w)
{
  // fprintf(stderr, "pointer_handle_button\n");

  Global *desktop = (Global*)data;

  if (state_w != WL_POINTER_BUTTON_STATE_RELEASED)
    return;

  if (!desktop->pointer_out_of_panel)
    return;

//   if (desktop->grid_visible)
//     launcher_grid_toggle (desktop->launcher_grid->window, desktop);

//   panel_window_leave_cb (NULL, NULL, desktop);
}

static void
pointer_handle_axis (void *data,
    struct wl_pointer *pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value)
{
  // fprintf(stderr, "pointer_handle_axis\n");

}

static const struct wl_pointer_listener pointer_listener = {
  pointer_handle_enter,
  pointer_handle_leave,
  pointer_handle_motion,
  pointer_handle_button,
  pointer_handle_axis,
};

static void
seat_handle_capabilities (void *data,
    struct wl_seat *seat,
    uint32_t caps)
{
  Global *desktop = (Global*)data;

  if ((caps & WL_SEAT_CAPABILITY_POINTER) && !desktop->pointer) {
    desktop->pointer = wl_seat_get_pointer(seat);
    wl_pointer_set_user_data (desktop->pointer, desktop);
    wl_pointer_add_listener(desktop->pointer, &pointer_listener,
          desktop);
  } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && desktop->pointer) {
    wl_pointer_destroy(desktop->pointer);
    desktop->pointer = NULL;
  }

  /* TODO: keyboard and touch */
	// if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !desktop->keyboard) {
	// 	desktop->keyboard = wl_seat_get_keyboard(seat);
	// 	wl_keyboard_set_user_data(desktop->keyboard, desktop);
	// 	wl_keyboard_add_listener(desktop->keyboard, &keyboard_listener,               //<------------------- hyjiang, setup keyboard listener
	// 				 desktop);
	// } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && desktop->keyboard) {
	// 	if (desktop->seat_version >= WL_KEYBOARD_RELEASE_SINCE_VERSION)
	// 		wl_keyboard_release(desktop->keyboard);
	// 	else
	// 		wl_keyboard_destroy(desktop->keyboard);
	// 	desktop->keyboard = NULL;
	// }
}

static void
seat_handle_name (void *data,
    struct wl_seat *seat,
    const char *name)
{
}

static const struct wl_seat_listener seat_listener = {
  seat_handle_capabilities,
  seat_handle_name
};

static void
registry_handle_global (void *data,
    struct wl_registry *registry,
    uint32_t name,
    const char *interface,
    uint32_t version)
{
  Global *d = (Global*)data;

  if (!strcmp (interface, "weston_desktop_shell"))
    {
      d->wshell = (weston_desktop_shell*)wl_registry_bind (registry, name,
          &weston_desktop_shell_interface, MIN(version, 1));
      weston_desktop_shell_add_listener (d->wshell, &wshell_listener, d);
      weston_desktop_shell_set_user_data (d->wshell, d);
    }
  else if (!strcmp (interface, "wl_output"))
    {
      /* TODO: create multiple outputs */
      d->output = (wl_output*)wl_registry_bind (registry, name,
          &wl_output_interface, 1);
    }
  else if (!strcmp (interface, "wl_seat"))
    {
    //   d->seat_version = MIN(version, 7);
      d->seat = (wl_seat*)wl_registry_bind (registry, name,
          &wl_seat_interface, 1);
      wl_seat_add_listener (d->seat, &seat_listener, d);
    }
  else if (!strcmp (interface, "shell_helper"))
    {
      d->helper = (shell_helper*)wl_registry_bind (registry, name,
          &shell_helper_interface, 1);
    }
}

static void
registry_handle_global_remove (void *data,
    struct wl_registry *registry,
    uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
  registry_handle_global,
  registry_handle_global_remove
};


Global::Global()
{
    // g_resources_register(maynard_get_resource());

    this->output = NULL;
    this->wshell = NULL;
    this->helper = NULL;
    this->seat = NULL;
    this->pointer = NULL;

    this->gdk_display = gdk_display_get_default();
    this->display = gdk_wayland_display_get_wl_display(this->gdk_display); 
    if (this->display == NULL)
    {
        fprintf(stderr, "failed to get display: %m\n");
        return;
    }
    this->registry = wl_display_get_registry(this->display);
    wl_registry_add_listener(this->registry, 
                             &registry_listener, this);
}


/* Expose callback for the drawing area */
static gboolean
draw_cb (GtkWidget *widget,
    cairo_t *cr,
    gpointer data)
{
  Global *desktop = (Global*)data;

  gdk_cairo_set_source_pixbuf (cr, desktop->background->pixbuf, 0, 0);
  cairo_paint (cr);

  return TRUE;
}


static GdkPixbuf *
scale_background (GdkPixbuf *original_pixbuf)
{
  /* Scale original_pixbuf so it mostly fits on the screen.
   * If the aspect ratio is different than a bit on the right or on the
   * bottom could be cropped out. */
  GdkDisplay *display = gdk_display_get_default ();
  /* There's no primary monitor on nested wayland so just use the
     first one for now */
  GdkMonitor *monitor = gdk_display_get_monitor (display, 0);
  GdkRectangle geom;
  gint original_width, original_height;
  gint final_width, final_height;
  gdouble ratio_horizontal, ratio_vertical, ratio;

  g_return_val_if_fail(monitor, NULL);

  gdk_monitor_get_geometry (monitor, &geom);

  original_width = gdk_pixbuf_get_width (original_pixbuf);
  original_height = gdk_pixbuf_get_height (original_pixbuf);

  ratio_horizontal = (double) geom.width / original_width;
  ratio_vertical = (double) geom.height / original_height;
  ratio = MAX (ratio_horizontal, ratio_vertical);

  final_width = ceil (ratio * original_width);
  final_height = ceil (ratio * original_height);

  return gdk_pixbuf_scale_simple (original_pixbuf,
      final_width, final_height, GDK_INTERP_BILINEAR);
}

static void
background_create (Global *desktop)
{
  GdkWindow *gdk_window;
  struct Element *background;
  const gchar *filename;
  GdkPixbuf *unscaled_background;
  const gchar *xpm_data[] = {"1 1 1 1", "_ c SteelBlue", "_"};

  background = (Element*)malloc (sizeof *background);
  memset (background, 0, sizeof *background);

  filename = g_getenv ("MAYNARD_BACKGROUND");
  if (filename && filename[0] != '\0')
    unscaled_background = gdk_pixbuf_new_from_file (filename, NULL);
  else
    unscaled_background = gdk_pixbuf_new_from_xpm_data (xpm_data);

  if (!unscaled_background)
    {
      g_message ("Could not load background (%s).",
          filename ? filename : "built-in");
      exit (EXIT_FAILURE);
    }

  background->pixbuf = scale_background (unscaled_background);
  g_object_unref (unscaled_background);

  background->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);    //hyjiang, gtk api GTK_WINDOW_TOPLEVEL

//   g_signal_connect (background->window, "destroy",
//       G_CALLBACK (destroy_cb), NULL);

  g_signal_connect (background->window, "draw",
      G_CALLBACK (draw_cb), desktop);

  // gtk_widget_add_events(background->window, GDK_KEY_PRESS_MASK);
  // g_signal_connect (G_OBJECT(background->window), "event", 
  //   G_CALLBACK (on_event), desktop);
  // g_signal_connect (G_OBJECT(background->window), "key-press-event", 
  //   G_CALLBACK (on_key_press), desktop);

  gtk_window_set_title (GTK_WINDOW (background->window), "maynard");
  gtk_window_set_decorated (GTK_WINDOW (background->window), FALSE);   //hyjinag, make windows have not decorated
  gtk_widget_realize (background->window);

  gdk_window = gtk_widget_get_window (background->window);
  gdk_wayland_window_set_use_custom_surface (gdk_window);              //hyjiang, *******

  background->surface = gdk_wayland_window_get_wl_surface (gdk_window);     //hyjiang, get wl_surface via gtk 3.0 api, latest version not found

  weston_desktop_shell_set_user_data (desktop->wshell, desktop);
  weston_desktop_shell_set_background (desktop->wshell, desktop->output,    //hyjiang, weston-desktop-shell protocol
      background->surface);

  desktop->background = background;

  gtk_widget_show_all (background->window);
}

int main(int argc, char *argv[])
{
    gdk_set_allowed_backends("wayland");


    Gtk::Main kit(argc, argv);


    // auto app =
    //     Gtk::Application::create(argc, argv,
    //                              "org.gtkmm.examples.base");

    Global* g = new Global();

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

    g->grid_visible = FALSE;
    g->system_visible = FALSE;
    g->volume_visible = FALSE;
    g->pointer_out_of_panel = FALSE;

    background_create(g);

    // return app->run(argc,argv);

    // Gtk::Window window;
    // window.set_default_size(200, 200);

    // return app->run(window);

    // gtk_main();

    Gtk::Main::run();


    return EXIT_SUCCESS;
}
