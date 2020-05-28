#ifndef MAYNARD_VERTICAL_CLOCK_H
#define MAYNARD_VERTICAL_CLOCK_H

#include <gtkmm.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define GNOME_DESKTOP_USE_UNSTABLE_API
#include <libgnome-desktop/gnome-wall-clock.h>

#ifdef  __cplusplus
}
#endif

class MaynardVerticalClock : public Gtk::Box
{
public:
    MaynardVerticalClock();
    virtual ~MaynardVerticalClock();

private:
    static void wall_clock_notify_cb(GnomeWallClock *wall_clock,
                                                           GParamSpec *pspec,
                                                           MaynardVerticalClock *self);

    Gtk::Label label;
    GnomeWallClock *wall_clock;

};

#endif //MAYNARD_VERTICAL_CLOCK_H
