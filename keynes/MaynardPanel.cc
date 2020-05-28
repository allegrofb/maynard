#include <iostream>
#include <algorithm> // std::max
#include "MaynardPanel.h"
#include "MaynardFavorites.h"
#include "MaynardVerticalClock.h"


MaynardPanel::MaynardPanel()
{
    Gtk::EventBox *ebox;
    Gtk::Box *main_box, *menu_box, *buttons_box;
    GtkWidget *button;
    Gtk::Button *pButton;

    /* window properties */
    set_title("maynard");
    set_decorated(FALSE);
    realize(); 

    /* make it black and slightly alpha */
    auto context = get_style_context();
    context->add_class("maynard-panel");

    /* main vbox */
    main_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
    add(*main_box);

    /* for the top buttons and vertical clock we have a few more
   * boxes. the hbox has two cells. in each cell there is a
   * GtkRevealer for hiding and showing the content. only one revealer
   * is ever visibile at one point and transitions happen at the same
   * time so the width stays constant (the animation duration is the
   * same). the first revealer contains another box which has the two
   * wifi and sound buttons. the second revealer has the vertical
   * clock widget.
   */

    /* GtkBoxes seem to eat up enter/leave events, so let's use an event
   * box for the entire thing. */
    ebox = Gtk::manage(new Gtk::EventBox());
    main_box->pack_start(*ebox, false, false, 0);
    ebox->signal_enter_notify_event()
        .connect(sigc::mem_fun(*this, &MaynardPanel::on_my_enter_notify_event));

    menu_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 0));
    ebox->add(*menu_box);

    /* revealer for the top buttons */
    revealer_buttons = Gtk::manage(new Gtk::Revealer());
    revealer_buttons->set_transition_type(Gtk::REVEALER_TRANSITION_TYPE_SLIDE_LEFT);
    revealer_buttons->set_reveal_child(true);
    menu_box->pack_start(*revealer_buttons, false, false, 0);

    /* the box for the top buttons */
    buttons_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
    revealer_buttons->add(*buttons_box);

    /* system button */
    ebox = Gtk::manage(new Gtk::EventBox());
    buttons_box->pack_start(*ebox, false, false, 0);
    button = gtk_button_new_from_icon_name("emblem-system-symbolic",
                                           GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_style_context_add_class(gtk_widget_get_style_context(button),
                                "maynard-system");
    gtk_style_context_remove_class(gtk_widget_get_style_context(button),
                                   "button");
    gtk_style_context_remove_class(gtk_widget_get_style_context(button),
                                   "image-button");
    pButton = Glib::wrap((GtkButton*)button);
    pButton->signal_clicked().connect(sigc::mem_fun(*this, &MaynardPanel::do_system_toggled));
    ebox->add(*pButton);
    ebox->signal_enter_notify_event()
        .connect(sigc::mem_fun(*this, &MaynardPanel::on_my_enter_notify_event));
    system_button = (GtkButton*)button;

    /* sound button */
    ebox = Gtk::manage(new Gtk::EventBox());
    buttons_box->pack_start(*ebox, false, false, 0);
    // button = gtk_button_new_from_icon_name(self->priv->volume_icon_name,
    //                                        GTK_ICON_SIZE_LARGE_TOOLBAR);
    button = gtk_button_new_from_icon_name("audio-volume-high-symbolic",
                                           GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_style_context_add_class(gtk_widget_get_style_context(button),
                                "maynard-audio");
    gtk_style_context_remove_class(gtk_widget_get_style_context(button),
                                   "button");
    gtk_style_context_remove_class(gtk_widget_get_style_context(button),
                                   "image-button");
    // g_signal_connect(button, "clicked",
    //                  G_CALLBACK(volume_button_clicked_cb), this);
    pButton = Glib::wrap((GtkButton*)button);
    pButton->signal_clicked().connect(sigc::mem_fun(*this, &MaynardPanel::do_volume_toggled));
    ebox->add(*pButton);
    ebox->signal_enter_notify_event()
        .connect(sigc::mem_fun(*this, &MaynardPanel::on_my_enter_notify_event));
    volume_button = (GtkButton*)button;

    /* revealer for the vertical clock */
    revealer_clock  = Gtk::manage(new Gtk::Revealer());
    revealer_clock->set_transition_type(Gtk::REVEALER_TRANSITION_TYPE_SLIDE_RIGHT);
    revealer_clock->set_reveal_child(false);
    menu_box->pack_start(*revealer_clock, false, false, 0);

    /* vertical clock */
    revealer_clock->add(*Gtk::manage(new MaynardVerticalClock()));

    /* end of the menu buttons and vertical clock */

    /* favorites */
    ebox = Gtk::manage(new Gtk::EventBox());
    main_box->pack_start(*ebox, false, false, 0);
    MaynardFavorites *favorites = Gtk::manage(new MaynardFavorites());
    ebox->add(*favorites);
    ebox->signal_enter_notify_event()
        .connect(sigc::mem_fun(*this, &MaynardPanel::on_my_enter_notify_event));
    favorites->app_launched.connect(sigc::mem_fun(*this, &MaynardPanel::do_favorite_launched));

    /* bottom app menu button */
    ebox = Gtk::manage(new Gtk::EventBox());
    main_box->pack_start(*ebox, false, false, 0);

    pButton = Gtk::manage( new Gtk::Button() );
    pButton->set_always_show_image(true);
    pButton->set_image_from_icon_name("view-grid-symbolic");
    // g_signal_connect(pButton->gobj(), "clicked",
    //                  G_CALLBACK(app_menu_button_clicked_cb), this);
    pButton->signal_clicked().connect(sigc::mem_fun(*this, &MaynardPanel::do_app_menu_toggled));
    ebox->add(*pButton);                     
    ebox->signal_enter_notify_event()
        .connect(sigc::mem_fun(*this, &MaynardPanel::on_my_enter_notify_event));

    /* done */
    hidden = false;
    volume_showing = false;

    #ifndef TEST_APP
    Wayland *g = Wayland::getInstance();
    g->panel.window = this->window;
    g->panel.surface = this->surface;
    g->panel.pixbuf = this->pixbuf;
    weston_desktop_shell_set_panel(g->wshell, g->output,
                                        this->surface);
    weston_desktop_shell_set_panel_position(g->wshell,
                                            WESTON_DESKTOP_SHELL_PANEL_POSITION_LEFT);
    shell_helper_set_panel (g->helper, g->panel.surface);
    show_all_children();
    #endif
}

MaynardPanel::~MaynardPanel()
{
    /*
  // These calls to Gtk::Widget::unparent() are necessary if MaynardPanel is
  // deleted before its children. But if you use a version of gtkmm where bug
  // https://bugzilla.gnome.org/show_bug.cgi?id=605728
  // has not been fixed (gtkmm 3.7.10 or earlier) and the children are deleted
  // before the container, these calls can make the program crash.
  // That's because on_remove() is not called, when the children are deleted.
  if (m_child_one)
    m_child_one->unparent();

  if (m_child_two)
    m_child_two->unparent();
*/
}

bool MaynardPanel::on_my_enter_notify_event(GdkEventCrossing *crossing_event)
{
    gboolean handled;
    g_signal_emit_by_name(this->gobj(), "enter-notify-event", crossing_event, &handled);
    return handled;
}

void MaynardPanel::do_app_menu_toggled()
{
    app_menu_toggled.emit();
}

void MaynardPanel::do_system_toggled()
{
    system_toggled.emit();
}

void MaynardPanel::do_volume_toggled()
{
    volume_toggled.emit();
}

void MaynardPanel::do_favorite_launched()
{
    favorite_launched.emit();
}
