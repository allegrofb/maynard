#ifndef MAYNARD_LAUNCHER_H
#define MAYNARD_LAUNCHER_H

#include <gtkmm.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include "../shell/shell-app-system.h"

#ifdef  __cplusplus
}
#endif

class MaynardLauncher : public Gtk::Window
{
public:
    MaynardLauncher();
    virtual ~MaynardLauncher();

private:

    static void installed_changed_cb(ShellAppSystem *app_system, MaynardLauncher* self);
    static GtkWidget* app_launcher_new_from_desktop_info (MaynardLauncher *self, GDesktopAppInfo *info);
    static void maynard_launcher_calculate (MaynardLauncher *self,
                                                            gint *grid_window_width,
                                                            gint *grid_window_height,
                                                            gint *grid_cols);

    // Gtk::Widget background;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Grid grid;

    ShellAppSystem *app_system;
};

#endif //MAYNARD_LAUNCHER_H
