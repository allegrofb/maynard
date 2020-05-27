#ifndef MAYNARD_FAVORITES_H
#define MAYNARD_FAVORITES_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <giomm/settings.h>

class MaynardFavorites : public Gtk::Box
{
public:
    MaynardFavorites();
    virtual ~MaynardFavorites();

    // Gtk::Box m_Box;
//     void set_child_widgets(Gtk::Widget &child_one, Gtk::Widget &child_two);

// protected:
//     //Overrides:
//     Gtk::SizeRequestMode get_request_mode_vfunc() const override;
//     void get_preferred_width_vfunc(int &minimum_width, int &natural_width) const override;
//     void get_preferred_height_for_width_vfunc(int width, int &minimum_height, int &natural_height) const override;
//     void get_preferred_height_vfunc(int &minimum_height, int &natural_height) const override;
//     void get_preferred_width_for_height_vfunc(int height, int &minimum_width, int &natural_width) const override;
//     void on_size_allocate(Gtk::Allocation &allocation) override;

//     void forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data) override;

//     void on_add(Gtk::Widget *child) override;
//     void on_remove(Gtk::Widget *child) override;
//     GType child_type_vfunc() const override;

//     Gtk::Widget *m_child_one;
//     Gtk::Widget *m_child_two;

private:
    void on_settings_changed(const Glib::ustring &key);
    void add_favorite(const Glib::ustring &favorite);
    Glib::RefPtr<Gio::Settings> m_Settings;
};

#endif //MAYNARD_FAVORITES_H
