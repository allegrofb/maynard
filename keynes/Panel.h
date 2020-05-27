#ifndef PANELWINDOW_H
#define PANELWINDOW_H

#include <gtkmm.h>
#include "WlWindow.h"

class PanelWindow : public WlWindow
{
public:
    PanelWindow();
    virtual ~PanelWindow();

protected:
    //Signal handlers:
    void on_color_button_color_set();
    void on_button_dialog_clicked();
    bool on_drawing_area_draw(const Cairo::RefPtr<Cairo::Context> &cr);

    Gtk::Box main_box;
    Gtk::Box menu_box;
    Gtk::Box buttons_box;

    Gtk::Revealer revealer_buttons; /* for the top buttons */
    Gtk::Revealer revealer_clock; /* for the vertical clock */
    Gtk::Button system_button;
    gboolean volume_showing;
    Gtk::Button volume_button;
    gchar *volume_icon_name;

};

#endif //PANELWINDOW_H
