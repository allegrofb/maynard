
#ifndef WLWINDOW_H
#define WLWINDOW_H

#include <gtkmm.h>
#include <gdk/gdkwayland.h>                      

class WlWindow : public Gtk::Window
{
public:
  WlWindow();
  virtual ~WlWindow();

public:
    GtkWidget *window;
    GdkPixbuf *pixbuf;
    struct wl_surface *surface;

};

#endif //WLWINDOW_H