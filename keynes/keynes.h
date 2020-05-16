
#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H




#include <gtk/gtk.h>          
#include <gdk/gdkwayland.h>   
#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"

struct Element
{
    GtkWidget *window;
    GdkPixbuf *pixbuf;
    struct wl_surface *surface;
};

class Global
{
public:
    Global();

public:
    struct wl_display *display;
    struct wl_registry *registry;
    struct weston_desktop_shell *wshell;
    struct wl_output *output;
    struct shell_helper *helper;

    struct wl_seat *seat;
    struct wl_pointer *pointer;

    GdkDisplay *gdk_display;

    struct Element *background;
    struct Element *panel;
    struct Element *curtain;
    struct Element *launcher_grid;
    struct Element *clock;

    guint initial_panel_timeout_id;
    guint hide_panel_idle_id;

    gboolean grid_visible;
    gboolean system_visible;
    gboolean volume_visible;
    gboolean pointer_out_of_panel;
};

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:
  //Signal handlers:
  void on_button_clicked();

  //Member widgets:
  Gtk::Button m_button;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H