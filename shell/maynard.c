/*
 * Copyright (c) 2013 Tiago Vignatti
 * Copyright (c) 2013-2014 Collabora Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>                             //hyjiang, libgtk-3-dev, gtk_*
#include <gdk/gdkwayland.h>                      //hyjiang, libgtk-3-dev, gdk_*
#include <gdk/gdkkeysyms.h>

#include "weston-desktop-shell-client-protocol.h"     //hyjiang, generated by wayland-scanner
#include "shell-helper-client-protocol.h"             //hyjiang, generated by wayland-scanner

#include "maynard-resources.h"     //resource_files = $(shell glib-compile-resources --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/maynard.gresource.xml)

#include "app-icon.h"
#include "clock.h"
#include "favorites.h"
#include "launcher.h"
#include "panel.h"
#include "vertical-clock.h"

#include <sys/mman.h>
#include <xkbcommon/xkbcommon.h>
#ifdef HAVE_XKBCOMMON_COMPOSE
#include <xkbcommon/xkbcommon-compose.h>
#endif


extern char **environ; /* defined by libc */

struct element {
  GtkWidget *window;
  GdkPixbuf *pixbuf;
  struct wl_surface *surface;
};

struct desktop {
  struct wl_display *display;
  struct wl_registry *registry;
  struct weston_desktop_shell *wshell;
  struct wl_output *output;
  struct shell_helper *helper;

  struct wl_seat *seat;
  struct wl_pointer *pointer;

  GdkDisplay *gdk_display;

  struct element *background;
  struct element *panel;
  struct element *curtain;
  struct element *launcher_grid;
  struct element *clock;

  guint initial_panel_timeout_id;
  guint hide_panel_idle_id;

  gboolean grid_visible;
  gboolean system_visible;
  gboolean volume_visible;
  gboolean pointer_out_of_panel;


  struct wl_keyboard *keyboard;
	struct xkb_context *xkb_context;
  uint32_t seat_version;
	struct {
		struct xkb_keymap *keymap;
		struct xkb_state *state;
#ifdef HAVE_XKBCOMMON_COMPOSE
		struct xkb_compose_table *compose_table;
		struct xkb_compose_state *compose_state;
#endif
		xkb_mod_mask_t control_mask;
		xkb_mod_mask_t alt_mask;
		xkb_mod_mask_t shift_mask;
	} xkb;
	uint32_t modifiers;

};


#define MOD_SHIFT_MASK		0x01
#define MOD_ALT_MASK		0x02
#define MOD_CONTROL_MASK	0x04

static gboolean panel_window_enter_cb (GtkWidget *widget,
    GdkEventCrossing *event, struct desktop *desktop);
static gboolean panel_window_leave_cb (GtkWidget *widget,
    GdkEventCrossing *event, struct desktop *desktop);

static gboolean
connect_enter_leave_signals (gpointer data)
{
  struct desktop *desktop = data;
  GList *l;

  g_signal_connect (desktop->panel->window, "enter-notify-event",
      G_CALLBACK (panel_window_enter_cb), desktop);
  g_signal_connect (desktop->panel->window, "leave-notify-event",
      G_CALLBACK (panel_window_leave_cb), desktop);

  g_signal_connect (desktop->clock->window, "enter-notify-event",
      G_CALLBACK (panel_window_enter_cb), desktop);
  g_signal_connect (desktop->clock->window, "leave-notify-event",
      G_CALLBACK (panel_window_leave_cb), desktop);

  g_signal_connect (desktop->launcher_grid->window, "enter-notify-event",
      G_CALLBACK (panel_window_enter_cb), desktop);
  g_signal_connect (desktop->launcher_grid->window, "leave-notify-event",
      G_CALLBACK (panel_window_leave_cb), desktop);

  return G_SOURCE_REMOVE;
}

static void
shell_configure (struct desktop *desktop,                          //hyjiang, callback by weston-desktop-shell protocol
    uint32_t edges,
    struct wl_surface *surface,
    int32_t width, int32_t height)
{
  int window_height;
  int grid_width, grid_height;

  gtk_widget_set_size_request (desktop->background->window,    //hyjiang, adjust background size
      width, height);

  /* TODO: make this height a little nicer */
  // window_height = height * MAYNARD_PANEL_HEIGHT_RATIO;
  // gtk_window_resize (GTK_WINDOW (desktop->panel->window),      //hyjiang, adjust panel size
  //     MAYNARD_PANEL_WIDTH, window_height);

  // maynard_launcher_calculate (MAYNARD_LAUNCHER (desktop->launcher_grid->window),
  //     &grid_width, &grid_height, NULL);
  // gtk_widget_set_size_request (desktop->launcher_grid->window,       //hyjiang, adjust lancher_grid size
  //     grid_width, grid_height);

  // shell_helper_move_surface (desktop->helper, desktop->panel->surface,
  //     0, (height - window_height) / 2);

  // gtk_window_resize (GTK_WINDOW (desktop->clock->window),             //hyjiang, adjust clock size
  //     MAYNARD_CLOCK_WIDTH, MAYNARD_CLOCK_HEIGHT);

  // shell_helper_move_surface (desktop->helper, desktop->clock->surface,
  //     MAYNARD_PANEL_WIDTH, (height - window_height) / 2);

  // shell_helper_move_surface (desktop->helper,
  //     desktop->launcher_grid->surface,
  //     - grid_width,
  //     ((height - window_height) / 2) + MAYNARD_CLOCK_HEIGHT);

  weston_desktop_shell_desktop_ready (desktop->wshell);

  /* TODO: why does the panel signal leave on drawing for
   * startup? we don't want to have to have this silly
   * timeout. */
  //g_timeout_add_seconds (1, connect_enter_leave_signals, desktop);    //hyjiang, setup enter leave signals
}

static void
weston_desktop_shell_configure (void *data,
    struct weston_desktop_shell *weston_desktop_shell,
    uint32_t edges,
    struct wl_surface *surface,
    int32_t width, int32_t height)
{
  shell_configure(data, edges, surface, width, height);     //hyjiang, callback by weston-desktop-shell protocol
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
launcher_grid_toggle (GtkWidget *widget,
    struct desktop *desktop)
{
  if (desktop->grid_visible)
    {
      shell_helper_slide_surface_back (desktop->helper,
          desktop->launcher_grid->surface);

      shell_helper_curtain (desktop->helper, desktop->curtain->surface, 0);
    }
  else
    {
      int width;

      gtk_widget_get_size_request (desktop->launcher_grid->window,
          &width, NULL);

      shell_helper_slide_surface (desktop->helper,
          desktop->launcher_grid->surface,
          width + MAYNARD_PANEL_WIDTH, 0);

      shell_helper_curtain (desktop->helper, desktop->curtain->surface, 1);
    }

  desktop->grid_visible = !desktop->grid_visible;
}

static void
launcher_grid_create (struct desktop *desktop)
{
  struct element *launcher_grid;
  GdkWindow *gdk_window;

  launcher_grid = malloc (sizeof *launcher_grid);
  memset (launcher_grid, 0, sizeof *launcher_grid);

  launcher_grid->window = maynard_launcher_new (desktop->background->window);
  gdk_window = gtk_widget_get_window (launcher_grid->window);
  launcher_grid->surface = gdk_wayland_window_get_wl_surface (gdk_window);

  gdk_wayland_window_set_use_custom_surface (gdk_window);
  shell_helper_add_surface_to_layer (desktop->helper,
      launcher_grid->surface,
      desktop->panel->surface);

  g_signal_connect (launcher_grid->window, "app-launched",
      G_CALLBACK (launcher_grid_toggle), desktop);

  gtk_widget_show_all (launcher_grid->window);

  desktop->launcher_grid = launcher_grid;
}

static void
volume_changed_cb (MaynardClock *clock,
    gdouble value,
    const gchar *icon_name,
    struct desktop *desktop)
{
  maynard_panel_set_volume_icon_name (
      MAYNARD_PANEL (desktop->panel->window), icon_name);
}

static GtkWidget *
clock_create (struct desktop *desktop)
{
  struct element *clock;
  GdkWindow *gdk_window;

  clock = malloc (sizeof *clock);
  memset (clock, 0, sizeof *clock);

  clock->window = maynard_clock_new ();

  g_signal_connect (clock->window, "volume-changed",
      G_CALLBACK (volume_changed_cb), desktop);

  gdk_window = gtk_widget_get_window (clock->window);
  clock->surface = gdk_wayland_window_get_wl_surface (gdk_window);

  gdk_wayland_window_set_use_custom_surface (gdk_window);
  shell_helper_add_surface_to_layer (desktop->helper, clock->surface,
      desktop->panel->surface);

  gtk_widget_show_all (clock->window);

  desktop->clock = clock;
}

static void
button_toggled_cb (struct desktop *desktop,
    gboolean *visible,
    gboolean *not_visible)
{
  *visible = !*visible;
  *not_visible = FALSE;

  if (desktop->system_visible)
    {
      maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
          MAYNARD_CLOCK_SECTION_SYSTEM);
      maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
          MAYNARD_PANEL_BUTTON_SYSTEM);
    }
  else if (desktop->volume_visible)
    {
      maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
          MAYNARD_CLOCK_SECTION_VOLUME);
      maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
          MAYNARD_PANEL_BUTTON_VOLUME);
    }
  else
    {
      maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
          MAYNARD_CLOCK_SECTION_CLOCK);
      maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
          MAYNARD_PANEL_BUTTON_NONE);
    }
}

static void
system_toggled_cb (GtkWidget *widget,
    struct desktop *desktop)
{
  button_toggled_cb (desktop,
      &desktop->system_visible,
      &desktop->volume_visible);
}

static void
volume_toggled_cb (GtkWidget *widget,
    struct desktop *desktop)
{
  button_toggled_cb (desktop,
      &desktop->volume_visible,
      &desktop->system_visible);
}

static gboolean
panel_window_enter_cb (GtkWidget *widget,
    GdkEventCrossing *event,
    struct desktop *desktop)
{
  if (desktop->initial_panel_timeout_id > 0)
    {
      g_source_remove (desktop->initial_panel_timeout_id);
      desktop->initial_panel_timeout_id = 0;
    }

  if (desktop->hide_panel_idle_id > 0)
    {
      g_source_remove (desktop->hide_panel_idle_id);
      desktop->hide_panel_idle_id = 0;
      return FALSE;
    }

  if (desktop->pointer_out_of_panel)
    {
      desktop->pointer_out_of_panel = FALSE;
      return FALSE;
    }

  shell_helper_slide_surface_back (desktop->helper,
      desktop->panel->surface);
  shell_helper_slide_surface_back (desktop->helper,
      desktop->clock->surface);

  maynard_panel_set_expand (MAYNARD_PANEL (desktop->panel->window), TRUE);

  return FALSE;
}

static gboolean
leave_panel_idle_cb (gpointer data)
{
  struct desktop *desktop = data;
  gint width;

  desktop->hide_panel_idle_id = 0;

  gtk_window_get_size (GTK_WINDOW (desktop->clock->window),
      &width, NULL);

  shell_helper_slide_surface (desktop->helper,
      desktop->panel->surface,
      MAYNARD_VERTICAL_CLOCK_WIDTH - MAYNARD_PANEL_WIDTH, 0);
  shell_helper_slide_surface (desktop->helper,
      desktop->clock->surface,
      MAYNARD_VERTICAL_CLOCK_WIDTH - MAYNARD_PANEL_WIDTH - width, 0);

  maynard_panel_set_expand (MAYNARD_PANEL (desktop->panel->window), FALSE);

  maynard_clock_show_section (MAYNARD_CLOCK (desktop->clock->window),
      MAYNARD_CLOCK_SECTION_CLOCK);
  maynard_panel_show_previous (MAYNARD_PANEL (desktop->panel->window),
      MAYNARD_PANEL_BUTTON_NONE);
  desktop->system_visible = FALSE;
  desktop->volume_visible = FALSE;
  desktop->pointer_out_of_panel = FALSE;

  return G_SOURCE_REMOVE;
}

static gboolean
panel_window_leave_cb (GtkWidget *widget,
    GdkEventCrossing *event,
    struct desktop *desktop)
{
  if (desktop->initial_panel_timeout_id > 0)
    {
      g_source_remove (desktop->initial_panel_timeout_id);
      desktop->initial_panel_timeout_id = 0;
    }

  if (desktop->hide_panel_idle_id > 0)
    return FALSE;

  if (desktop->grid_visible)
    {
      desktop->pointer_out_of_panel = TRUE;
      return FALSE;
    }

  desktop->hide_panel_idle_id = g_idle_add (leave_panel_idle_cb, desktop);

  return FALSE;
}

static gboolean
panel_hide_timeout_cb (gpointer data)
{
  struct desktop *desktop = data;

  panel_window_leave_cb (NULL, NULL, desktop);

  return G_SOURCE_REMOVE;
}

static void
favorite_launched_cb (MaynardPanel *panel,
    struct desktop *desktop)
{
  if (desktop->grid_visible)
    launcher_grid_toggle (desktop->launcher_grid->window, desktop);

  panel_window_leave_cb (NULL, NULL, desktop);
}

static void
panel_create (struct desktop *desktop)           //hyjiang
{
  struct element *panel;
  GdkWindow *gdk_window;

  panel = malloc (sizeof *panel);
  memset (panel, 0, sizeof *panel);

  panel->window = maynard_panel_new ();

  g_signal_connect (panel->window, "app-menu-toggled",   //hyjiang, like QT signal/slot
      G_CALLBACK (launcher_grid_toggle), desktop);
  g_signal_connect (panel->window, "system-toggled",
      G_CALLBACK (system_toggled_cb), desktop);
  g_signal_connect (panel->window, "volume-toggled",
      G_CALLBACK (volume_toggled_cb), desktop);
  g_signal_connect (panel->window, "favorite-launched",
      G_CALLBACK (favorite_launched_cb), desktop);

  desktop->initial_panel_timeout_id =
    g_timeout_add_seconds (2, panel_hide_timeout_cb, desktop);

  /* set it up as the panel */
  gdk_window = gtk_widget_get_window (panel->window);
  gdk_wayland_window_set_use_custom_surface (gdk_window);

  panel->surface = gdk_wayland_window_get_wl_surface (gdk_window);

  weston_desktop_shell_set_user_data (desktop->wshell, desktop);
  weston_desktop_shell_set_panel (desktop->wshell, desktop->output,   //hyjiang, weston-desktop-shell protocol
      panel->surface);
  weston_desktop_shell_set_panel_position (desktop->wshell,
     WESTON_DESKTOP_SHELL_PANEL_POSITION_LEFT);

  shell_helper_set_panel (desktop->helper, panel->surface);

  gtk_widget_show_all (panel->window);

  desktop->panel = panel;
}

/* Expose callback for the drawing area */
static gboolean
draw_cb (GtkWidget *widget,
    cairo_t *cr,
    gpointer data)
{
  struct desktop *desktop = data;

  gdk_cairo_set_source_pixbuf (cr, desktop->background->pixbuf, 0, 0);
  cairo_paint (cr);

  return TRUE;
}

/* Destroy handler for the window */
static void
destroy_cb (GObject *object,
    gpointer data)
{
  gtk_main_quit ();
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


void change_color(struct desktop *desktop)
{
  GdkPixbuf *unscaled_background;
  const gchar *xpm_data[] = {"1 1 1 1", "_ c Cyan", "_"};
  unscaled_background = gdk_pixbuf_new_from_xpm_data(xpm_data);

  if (!unscaled_background)
  {
    g_message("Could not load background (%s).",
              "built-in");
    exit(EXIT_FAILURE);
  }

  desktop->background->pixbuf = scale_background(unscaled_background);
  g_object_unref(unscaled_background);

}

static gboolean
on_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  printf("event: %d\n", event->type);

  return FALSE; 
}

static gboolean
on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  struct desktop *desktop = user_data;
  printf("key pressed: %s\n", "ttt");

  switch (event->keyval)
  {
    case GDK_KEY_p:
      // printf("key pressed: %s\n", "p");
      // g_print("key pressed: %s\n", "p");
      // gtk_main_quit();
      change_color(desktop);
      break;
    case GDK_KEY_S:
    case GDK_KEY_s:
      if (event->state & GDK_SHIFT_MASK)
      {
        printf("key pressed: %s\n", "shift + s");
      }
      else if (event->state & GDK_CONTROL_MASK)
      {
        printf("key pressed: %s\n", "ctrl + s");
      }
      else
      {
        printf("key pressed: %s\n", "s");
      }
      break;
    case GDK_KEY_M:
    case GDK_KEY_m:
      if (event->state & GDK_SHIFT_MASK)
      {
        printf("key pressed: %s\n", "shift + m");
      }
      else if (event->state & GDK_CONTROL_MASK)
      {
        printf("key pressed: %s\n", "ctrl + m");
      }
      else
      {
        printf("key pressed: %s\n", "m");
      }
      break;

    default:
      return FALSE; 
  }

  return FALSE; 
}



static void
background_create (struct desktop *desktop)
{
  GdkWindow *gdk_window;
  struct element *background;
  const gchar *filename;
  GdkPixbuf *unscaled_background;
  const gchar *xpm_data[] = {"1 1 1 1", "_ c SteelBlue", "_"};

  background = malloc (sizeof *background);
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

  g_signal_connect (background->window, "destroy",
      G_CALLBACK (destroy_cb), NULL);

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

static void
curtain_create (struct desktop *desktop)
{
  GdkWindow *gdk_window;
  struct element *curtain;

  curtain = malloc (sizeof *curtain);
  memset (curtain, 0, sizeof *curtain);

  curtain->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (curtain->window), "maynard");
  gtk_window_set_decorated (GTK_WINDOW (curtain->window), FALSE);
  gtk_widget_set_size_request (curtain->window, 8192, 8192);
  gtk_widget_realize (curtain->window);

  gdk_window = gtk_widget_get_window (curtain->window);
  gdk_wayland_window_set_use_custom_surface (gdk_window);

  curtain->surface = gdk_wayland_window_get_wl_surface (gdk_window);

  desktop->curtain = curtain;

  gtk_widget_show_all (curtain->window);
}

static void
css_setup (struct desktop *desktop)
{
  GtkCssProvider *provider;
  GFile *file;
  GError *error = NULL;

  provider = gtk_css_provider_new ();

  file = g_file_new_for_uri ("resource:///org/raspberry-pi/maynard/style.css");

  if (!gtk_css_provider_load_from_file (provider, file, &error))    //hyjiang, gtk api load css
    {
      g_warning ("Failed to load CSS file: %s", error->message);
      g_clear_error (&error);
      g_object_unref (file);
      return;
    }

  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
      GTK_STYLE_PROVIDER (provider), 600);

  g_object_unref (file);
}


static void
pointer_handle_enter (void *data,
    struct wl_pointer *pointer,
    uint32_t serial,
    struct wl_surface *surface,
    wl_fixed_t sx_w,
    wl_fixed_t sy_w)
{
  fprintf(stderr, "pointer_handle_enter\n");
}

static void
pointer_handle_leave (void *data,
    struct wl_pointer *pointer,
    uint32_t serial,
    struct wl_surface *surface)
{
  fprintf(stderr, "pointer_handle_leave\n");
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
  fprintf(stderr, "pointer_handle_button\n");

  struct desktop *desktop = data;

  if (state_w != WL_POINTER_BUTTON_STATE_RELEASED)
    return;

  if (!desktop->pointer_out_of_panel)
    return;

  if (desktop->grid_visible)
    launcher_grid_toggle (desktop->launcher_grid->window, desktop);

  panel_window_leave_cb (NULL, NULL, desktop);
}

static void
pointer_handle_axis (void *data,
    struct wl_pointer *pointer,
    uint32_t time,
    uint32_t axis,
    wl_fixed_t value)
{
  fprintf(stderr, "pointer_handle_axis\n");

}

static const struct wl_pointer_listener pointer_listener = {
  pointer_handle_enter,
  pointer_handle_leave,
  pointer_handle_motion,
  pointer_handle_button,
  pointer_handle_axis,
};



static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
		       uint32_t format, int fd, uint32_t size)
{
  fprintf(stderr,"keyboard_handle_keymap %d\n",__LINE__);
	struct desktop *input = data;
	struct xkb_keymap *keymap;
	struct xkb_state *state;
#ifdef HAVE_XKBCOMMON_COMPOSE
	struct xkb_compose_table *compose_table;
	struct xkb_compose_state *compose_state;
#endif
	char *locale;
	char *map_str;

	if (!data) {
		close(fd);
		return;
	}
  fprintf(stderr,"%d format=%d\n",__LINE__,format);

	if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
		close(fd);
		return;
	}
  fprintf(stderr,"%d\n",__LINE__);

	map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (map_str == MAP_FAILED) {
		close(fd);
		return;
	}
  fprintf(stderr,"%d\n",__LINE__);

	/* Set up XKB keymap */
	keymap = xkb_keymap_new_from_string(input->xkb_context,
					    map_str,
					    XKB_KEYMAP_FORMAT_TEXT_V1,
					    0);
  fprintf(stderr,"%d\n",__LINE__);

	munmap(map_str, size);
    fprintf(stderr,"%d\n",__LINE__);

	close(fd);
  fprintf(stderr,"%d\n",__LINE__);

	if (!keymap) {
		fprintf(stderr, "failed to compile keymap\n");
		return;
	}
  fprintf(stderr,"%d\n",__LINE__);

	/* Set up XKB state */
	state = xkb_state_new(keymap);
	if (!state) {
		fprintf(stderr, "failed to create XKB state\n");
		xkb_keymap_unref(keymap);
		return;
	}
  fprintf(stderr,"%d\n",__LINE__);

	/* Look up the preferred locale, falling back to "C" as default */
	if (!(locale = getenv("LC_ALL")))
		if (!(locale = getenv("LC_CTYPE")))
			if (!(locale = getenv("LANG")))
				locale = "C";

	/* Set up XKB compose table */
#ifdef HAVE_XKBCOMMON_COMPOSE
	compose_table =
		xkb_compose_table_new_from_locale(input->display->xkb_context,
						  locale,
						  XKB_COMPOSE_COMPILE_NO_FLAGS);
	if (compose_table) {
		/* Set up XKB compose state */
		compose_state = xkb_compose_state_new(compose_table,
					      XKB_COMPOSE_STATE_NO_FLAGS);
		if (compose_state) {
			xkb_compose_state_unref(input->xkb.compose_state);
			xkb_compose_table_unref(input->xkb.compose_table);
			input->xkb.compose_state = compose_state;
			input->xkb.compose_table = compose_table;
		} else {
			fprintf(stderr, "could not create XKB compose state.  "
				"Disabiling compose.\n");
			xkb_compose_table_unref(compose_table);
			compose_table = NULL;
		}
	} else {
		fprintf(stderr, "could not create XKB compose table for locale '%s'.  "
			"Disabiling compose\n", locale);
	}
#endif
      fprintf(stderr,"%d\n",__LINE__);

	xkb_keymap_unref(input->xkb.keymap);
        fprintf(stderr,"%d\n",__LINE__);

	xkb_state_unref(input->xkb.state);
        fprintf(stderr,"%d\n",__LINE__);

	input->xkb.keymap = keymap;
	input->xkb.state = state;

	input->xkb.control_mask =
		1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Control");
	input->xkb.alt_mask =
		1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Mod1");
	input->xkb.shift_mask =
		1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Shift");

      fprintf(stderr,"%d\n",__LINE__);

}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface,
		      struct wl_array *keys)
{
    fprintf(stderr,"keyboard_handle_enter %d\n",__LINE__);

}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface)
{
    fprintf(stderr,"keyboard_handle_leave %d\n",__LINE__);

}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
			  uint32_t serial, uint32_t mods_depressed,
			  uint32_t mods_latched, uint32_t mods_locked,
			  uint32_t group)
{
    fprintf(stderr,"keyboard_handle_modifiers %d\n",__LINE__);

  struct desktop *input = data;
	xkb_mod_mask_t mask;

	/* If we're not using a keymap, then we don't handle PC-style modifiers */
	if (!input->xkb.keymap)
		return;
    fprintf(stderr,"%d\n",__LINE__);

	xkb_state_update_mask(input->xkb.state, mods_depressed, mods_latched,
			      mods_locked, 0, 0, group);
	mask = xkb_state_serialize_mods(input->xkb.state,
					XKB_STATE_MODS_DEPRESSED |
					XKB_STATE_MODS_LATCHED);
	input->modifiers = 0;
	if (mask & input->xkb.control_mask)
		input->modifiers |= MOD_CONTROL_MASK;
	if (mask & input->xkb.alt_mask)
		input->modifiers |= MOD_ALT_MASK;
	if (mask & input->xkb.shift_mask)
		input->modifiers |= MOD_SHIFT_MASK;

}

static void
keyboard_handle_repeat_info(void *data, struct wl_keyboard *keyboard,
			    int32_t rate, int32_t delay)
{
    fprintf(stderr,"keyboard_handle_repeat_info %d\n",__LINE__);

}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,  
		    uint32_t serial, uint32_t time, uint32_t key,
		    uint32_t state_w)
{
  fprintf(stderr,"keyboard_handle_key %d\n",__LINE__);

	struct desktop *input = data;
	// struct window *window = input->keyboard_focus;
	uint32_t code, num_syms;
	enum wl_keyboard_key_state state = state_w;
	const xkb_keysym_t *syms;
	xkb_keysym_t sym;
	struct itimerspec its;

	// input->display->serial = serial;
	code = key + 8;
	// if (!window || !input->xkb.state)
	// 	return;

	/* We only use input grabs for pointer events for now, so just
	 * ignore key presses if a grab is active.  We expand the key
	 * event delivery mechanism to route events to widgets to
	 * properly handle key grabs.  In the meantime, this prevents
	 * key event delivery while a grab is active. */
	// if (input->grab && input->grab_button == 0)
	// 	return;

	num_syms = xkb_state_key_get_syms(input->xkb.state, code, &syms);

	sym = XKB_KEY_NoSymbol;
	if (num_syms == 1)
		sym = syms[0];

  g_print("keyboard_handle_key: %d\n", sym);
  printf("keyboard_handle_key: %d\n", sym);

	if (sym == XKB_KEY_F5 && input->modifiers == MOD_ALT_MASK) {
		if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
			// window_set_maximized(window, !window->maximized);
      ;
	} 
  // else if (sym == XKB_KEY_F11 &&
	// 	   window->fullscreen_handler &&
	// 	   state == WL_KEYBOARD_KEY_STATE_PRESSED) {
	// 	window->fullscreen_handler(window, window->user_data);
	// } 
  else if (sym == XKB_KEY_F4 &&
		   input->modifiers == MOD_ALT_MASK &&
		   state == WL_KEYBOARD_KEY_STATE_PRESSED) 
  {
		// window_close(window);
	} 
  // else if (window->key_handler) 
  // {
	// 	if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
	// 		sym = process_key_press(sym, input);

	// 	(*window->key_handler)(window, input, time, key,  
	// 			       sym, state, window->user_data);
	// }

	// if (state == WL_KEYBOARD_KEY_STATE_RELEASED &&
	//     key == input->repeat_key) {
	// 	toytimer_disarm(&input->repeat_timer);
	// } else if (state == WL_KEYBOARD_KEY_STATE_PRESSED &&
	// 	   xkb_keymap_key_repeats(input->xkb.keymap, code)) {
	// 	input->repeat_sym = sym;
	// 	input->repeat_key = key;
	// 	input->repeat_time = time;
	// 	its.it_interval.tv_sec = input->repeat_rate_sec;
	// 	its.it_interval.tv_nsec = input->repeat_rate_nsec;
	// 	its.it_value.tv_sec = input->repeat_delay_sec;
	// 	its.it_value.tv_nsec = input->repeat_delay_nsec;
	// 	toytimer_arm(&input->repeat_timer, &its);
	// }
}

static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
	keyboard_handle_repeat_info

};

static void
seat_handle_capabilities (void *data,
    struct wl_seat *seat,
    enum wl_seat_capability caps)
{
  struct desktop *desktop = data;

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
	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !desktop->keyboard) {
		desktop->keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_set_user_data(desktop->keyboard, desktop);
		wl_keyboard_add_listener(desktop->keyboard, &keyboard_listener,               //<------------------- hyjiang, setup keyboard listener
					 desktop);
	} else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && desktop->keyboard) {
		if (desktop->seat_version >= WL_KEYBOARD_RELEASE_SINCE_VERSION)
			wl_keyboard_release(desktop->keyboard);
		else
			wl_keyboard_destroy(desktop->keyboard);
		desktop->keyboard = NULL;
	}
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
  struct desktop *d = data;

  if (!strcmp (interface, "weston_desktop_shell"))
    {
      d->wshell = wl_registry_bind (registry, name,           //hyjiang, weston-desktop-shell protocol, interface
          &weston_desktop_shell_interface, MIN(version, 1));
      weston_desktop_shell_add_listener (d->wshell, &wshell_listener, d);
      weston_desktop_shell_set_user_data (d->wshell, d);
    }
  else if (!strcmp (interface, "wl_output"))
    {
      /* TODO: create multiple outputs */
      d->output = wl_registry_bind (registry, name,
          &wl_output_interface, 1);
    }
  else if (!strcmp (interface, "wl_seat"))
    {
      d->seat_version = MIN(version, 7);
      d->seat = wl_registry_bind (registry, name,
          &wl_seat_interface, 1);
      wl_seat_add_listener (d->seat, &seat_listener, d);
    }
  else if (!strcmp (interface, "shell_helper"))
    {
      d->helper = wl_registry_bind (registry, name,
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

static void grab_surface_create(struct desktop *desktop)
{

  struct wl_surface *s;

  GdkWindow *gdk_window;
  struct element *curtain;

  curtain = malloc (sizeof *curtain);
  memset (curtain, 0, sizeof *curtain);

  curtain->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (curtain->window), "maynard2");
  gtk_window_set_decorated (GTK_WINDOW (curtain->window), FALSE);
  gtk_widget_set_size_request (curtain->window, 8192, 8192);
  gtk_widget_realize (curtain->window);

  gdk_window = gtk_widget_get_window (curtain->window);
  gdk_wayland_window_set_use_custom_surface (gdk_window);

  curtain->surface = gdk_wayland_window_get_wl_surface (gdk_window);

  desktop->curtain = curtain;

  gtk_widget_show_all (curtain->window);
  weston_desktop_shell_set_grab_surface(desktop->wshell, curtain->surface);


}

int
main (int argc,
    char *argv[])
{
  struct desktop *desktop;

  gdk_set_allowed_backends ("wayland");

  gtk_init (&argc, &argv);                         //hyjiang

  g_resources_register (maynard_get_resource ());

  desktop = malloc (sizeof *desktop);
  desktop->output = NULL;
  desktop->wshell = NULL;
  desktop->helper = NULL;
  desktop->seat = NULL;
  desktop->pointer = NULL;
  desktop->keyboard = NULL;

  desktop->gdk_display = gdk_display_get_default ();
  desktop->display =
    gdk_wayland_display_get_wl_display (desktop->gdk_display);   //hyjiang, gtk api get wl_display
  if (desktop->display == NULL)
    {
      fprintf (stderr, "failed to get display: %m\n");
      return -1;
    }


	desktop->xkb_context = xkb_context_new(0);
	if (desktop->xkb_context == NULL) {
		fprintf(stderr, "Failed to create XKB context\n");
    free(desktop);
		return -1;
	}
  desktop->xkb.keymap = NULL;
  desktop->xkb.state = NULL;
  

  desktop->registry = wl_display_get_registry (desktop->display);
  wl_registry_add_listener (desktop->registry,                   //hyjiang, register listener, initial global variable, likely weston-desktop-shell interface
      &registry_listener, desktop);

  /* Wait until we have been notified about the compositor,
   * shell, and shell helper objects */
  if (!desktop->output || !desktop->wshell || !desktop->helper)
    wl_display_roundtrip (desktop->display);
  if (!desktop->output || !desktop->wshell || !desktop->helper)
    {
      fprintf (stderr, "could not find output, shell or helper modules\n");
      fprintf (stderr, "output: %p, wshell: %p, helper: %p\n",
               desktop->output, desktop->wshell, desktop->helper);
      return -1;
    }

  desktop->grid_visible = FALSE;
  desktop->system_visible = FALSE;
  desktop->volume_visible = FALSE;
  desktop->pointer_out_of_panel = FALSE;

  css_setup (desktop);
  background_create (desktop);                //hyjiang, create background, and info server via weston-desktop-shell protocol 
  //curtain_create (desktop);                   //hyjiang, related with shell-helper protocol, show the installed app box ??

  /* panel needs to be first so the clock and launcher grid can
   * be added to its layer */
  //panel_create (desktop);                     //hyjiang, left side, have vertial button, click last buton showing curtain ??
  // clock_create (desktop);
  //launcher_grid_create (desktop);
  //grab_surface_create (desktop);

  gtk_main ();

  /* TODO cleanup */
	xkb_context_unref(desktop->xkb_context);


  return EXIT_SUCCESS;
}
