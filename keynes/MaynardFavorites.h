#ifndef MAYNARD_FAVORITES_H
#define MAYNARD_FAVORITES_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <giomm/settings.h>


#ifdef  __cplusplus
extern "C" {
#endif

#include "../shell/app-icon.h"

#ifdef  __cplusplus
}
#endif

class MaynardFavorites : public Gtk::Box
{
public:
    MaynardFavorites();
    virtual ~MaynardFavorites();

    sigc::signal<void> app_launched;

private:
    void do_app_launched();
    void on_favorite_clicked();

private:
    void on_settings_changed(const Glib::ustring &key);
    void add_favorite(const Glib::ustring &favorite);
    Glib::RefPtr<Gio::Settings> m_Settings;
    GtkWidget *m_Button;
};

#endif //MAYNARD_FAVORITES_H
