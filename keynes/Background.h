
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <gtkmm.h>
#include <gdk/gdkwayland.h>                      

class BackgroundWindow : public Gtk::Window
{
public:
  BackgroundWindow();
  virtual ~BackgroundWindow();

public:
    GtkWidget *window;
    GdkPixbuf *pixbuf;
    struct wl_surface *surface;

protected:
  //Signal handlers:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

};

#endif //BACKGROUND_H