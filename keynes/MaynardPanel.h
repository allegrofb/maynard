#ifndef MAYNARD_PANEL_H
#define MAYNARD_PANEL_H

#include <gtkmm.h>

#ifdef TEST_APP
class MaynardPanel : public Gtk::Window
#else
#include "WlWindow.h"
class MaynardPanel : public WlWindow
#endif
{
public:
    MaynardPanel();
    virtual ~MaynardPanel();

public:
    sigc::signal<void> app_menu_toggled;
    sigc::signal<void> system_toggled;
    sigc::signal<void> volume_toggled;
    sigc::signal<void> favorite_launched;

private:
    void do_app_menu_toggled();
    void do_system_toggled();
    void do_volume_toggled();
    void do_favorite_launched();

    bool on_my_enter_notify_event(GdkEventCrossing* crossing_event);

    gboolean hidden;
    gboolean volume_showing;

    Gtk::Revealer *revealer_buttons;
    Gtk::Revealer *revealer_clock;
    GtkButton *system_button;
    GtkButton *volume_button;
};


#endif //MAYNARD_PANEL_H
