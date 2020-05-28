#include <iostream>
#include <algorithm> // std::max
#include "MaynardFavorites.h"
#include <gio/gdesktopappinfo.h>

MaynardFavorites::MaynardFavorites()
    : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
    set_has_window(false);
    set_redraw_on_allocate(false);
    m_Settings = Gio::Settings::create("org.raspberrypi.maynard");
    m_Settings->signal_changed().connect(sigc::mem_fun(*this,
              &MaynardFavorites::on_settings_changed));
    on_settings_changed("favorites");
}

MaynardFavorites::~MaynardFavorites()
{
    /*
  // These calls to Gtk::Widget::unparent() are necessary if MaynardFavorites is
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

void MaynardFavorites::do_app_launched()
{
    app_launched.emit();
}

void MaynardFavorites::on_settings_changed(const Glib::ustring &key)
{
    this->foreach([&](Gtk::Widget& widget){
        this->remove(widget);
    });

    auto favorites = m_Settings->get_string_array(key);
    for(auto i=favorites.begin();i!=favorites.end();++i)
    {
        // std::cout << "*** " << *i << std::endl;
        add_favorite(*i);
    }
}

void MaynardFavorites::on_favorite_clicked()
{

    GAppInfo *info = (GAppInfo*)g_object_get_data(G_OBJECT(m_Button), "info");
    GError *error = NULL;

    g_app_info_launch(info, NULL, NULL, &error);
    if (error)
    {
        g_warning("Could not launch app %s: %s",
                  g_app_info_get_name(info),
                  error->message);
        g_clear_error(&error);
    }

    do_app_launched();
}

void MaynardFavorites::add_favorite(const Glib::ustring &favorite)
{
    // Gtk::Button* pButton = Gtk::manage( new Gtk::Button() );
    // pButton->set_always_show_image(true);
    // pButton->set_image_from_icon_name(favorite);
    // this->pack_end(*pButton, false, false, 0);

    GDesktopAppInfo *info;
    GtkWidget *button, *image;
    GIcon *icon;

    info = g_desktop_app_info_new(favorite.c_str());

    if (!info)
        return;

    icon = g_app_info_get_icon(G_APP_INFO(info));

    button = maynard_app_icon_new_from_gicon(icon);
    m_Button = button;
    g_object_set_data_full(G_OBJECT(button), "info", info, g_object_unref);

    Gtk::Button *pButton = Glib::wrap((GtkButton*)button);
    // g_signal_connect(button, "clicked", G_CALLBACK(favorite_clicked), this);
    pButton->signal_clicked().connect(sigc::mem_fun(*this, &MaynardFavorites::on_favorite_clicked) );

    this->pack_end(*Gtk::manage(pButton), false, false, 0);

}
