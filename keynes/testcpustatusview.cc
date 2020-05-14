#include "config.h"
#define WMAPP
#include "appnames.h"
#include "yfull.h"
#include "wmprog.h"
#include "wmwinmenu.h"
#include "workspaces.h"
#include "wmapp.h"
#include "wmframe.h"
#include "wmswitch.h"
#include "wmstatus.h"
#include "wmabout.h"
#include "wmdialog.h"
#include "wmconfig.h"
#include "wmwinlist.h"
#include "wmtaskbar.h"
#include "wmsession.h"
#include "wpixres.h"
#include "sysdep.h"
#include "ylocale.h"
#include "yprefs.h"
#include "prefs.h"
#include "udir.h"
#include "ascii.h"
#include "ypaths.h"
#include "yxcontext.h"
#ifdef CONFIG_XFREETYPE
#include <ft2build.h>
#include <X11/Xft/Xft.h>
#endif
#undef override
#include <X11/Xproto.h>
#include "intl.h"


char const *ApplicationName("IceWM");





#include <gtk/gtk.h>
#include <assert.h>
#include "widget.h"


//for WPixRes::initPixmaps();
#include "wpixres.h"

//for CPUStatus
#include "ytimer.h"
#include "yaction.h"
#include "ypointer.h"
#include "ywindow.h"
#include "ymenu.h"
#include "applet.h"
#include "acpustatus.h"

void CPUStatusView()
{
   GtkWidget *window;
   GtkWidget *cpu;

   window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit ), NULL);

   CPUStatus* status = new CPUStatus(NULL, NULL);
   cpu = status->getWidget();
   gtk_container_add(GTK_CONTAINER(window), cpu);
   gtk_widget_show_all(window);
}


int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);
    debug = 1;

    // YApplication app(&argc, &argv);
    // manager = new YWindowManager(
    //     NULL, NULL, NULL, nullptr, 0);
    // manager->initWorkspaces();

    WPixRes::initPixmaps();

    // iconView();
    // mycpu();
    CPUStatusView();
    // ClockView();
    // ObjectButtonView();
    // WorkspacePaneView();
    // StartMenuView(&app);
    gtk_main();

    return 0;
}


// vim: set sw=4 ts=4 et:
