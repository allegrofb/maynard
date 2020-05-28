#include "MaynardVerticalClock.h"

#define MAYNARD_PANEL_WIDTH 56
#define MAYNARD_PANEL_HEIGHT_RATIO 0.73
#define MAYNARD_CLOCK_WIDTH (MAYNARD_PANEL_WIDTH * 2.6)
#define MAYNARD_CLOCK_HEIGHT (MAYNARD_PANEL_WIDTH * 2)
#define GRID_ITEM_WIDTH 114
#define GRID_ITEM_HEIGHT 114
#define MAYNARD_VERTICAL_CLOCK_WIDTH 25

MaynardVerticalClock::MaynardVerticalClock()
    : Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
      label("")
{
    wall_clock = (GnomeWallClock*)g_object_new(GNOME_TYPE_WALL_CLOCK, NULL);
    g_signal_connect(wall_clock, "notify::clock",
                     G_CALLBACK(wall_clock_notify_cb), this);

    /* a label just to pad things out to the correct width */
    gint width = MAYNARD_PANEL_WIDTH - MAYNARD_VERTICAL_CLOCK_WIDTH;

    Gtk::Label *padding = Gtk::manage(new Gtk::Label(""));
    auto context = padding->get_style_context();
    context->add_class("maynard-clock");
    padding->set_size_request(width, -1);

    pack_start(*padding, false, false, 0);

    /* the actual clock label */
    auto context2 = label.get_style_context();
    context2->add_class("maynard-clock");
    label.set_justify(Gtk::JUSTIFY_CENTER);
    label.set_size_request(MAYNARD_VERTICAL_CLOCK_WIDTH, -1);
    pack_start (label, false, false, 0);

    wall_clock_notify_cb (wall_clock, NULL, this);
}

void MaynardVerticalClock::wall_clock_notify_cb(GnomeWallClock *wall_clock,
                                                GParamSpec *pspec,
                                                MaynardVerticalClock *self)
{
    GDateTime *datetime;
    gchar *str;

    datetime = g_date_time_new_now_local();

    str = g_date_time_format(datetime,
                             "<span font=\"Droid Sans 12\">%H\n"
                             ":\n"
                             "%M</span>");

    self->label.set_markup(str);

    g_free(str);
    g_date_time_unref(datetime);
}

MaynardVerticalClock::~MaynardVerticalClock()
{

}
