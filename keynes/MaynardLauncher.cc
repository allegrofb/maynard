#include <iostream>
#include <algorithm> // std::max
#include "MaynardLauncher.h"
#include "Config.h"

MaynardLauncher::MaynardLauncher()
{
    set_title("maynard");
    set_decorated(FALSE);
    realize();                //hyjiang

    auto context = get_style_context();
    context->add_class("maynard-grid");

    add(scrolled_window);

    scrolled_window.add(grid);

    /* fill the grid with apps */
    app_system = shell_app_system_get_default();
    g_signal_connect(this->app_system, "installed-changed",
                     G_CALLBACK(installed_changed_cb), this);

    /* refill the grid if the background is changed */
    // g_assert(self->priv->background != NULL);
    // g_signal_connect(self->priv->background, "size-allocate",
    //                  G_CALLBACK(background_size_allocate_cb), self);

    /* now actually fill the grid */
    installed_changed_cb(app_system, this);

    #ifndef TEST_APP
    Wayland* g = Wayland::getInstance();
    g->launcher_grid.window = this->window;
    g->launcher_grid.surface = this->surface;
    g->launcher_grid.pixbuf = this->pixbuf;
    shell_helper_add_surface_to_layer(g->helper,
                                      g->launcher_grid.surface,
                                      g->panel.surface);
    show_all_children();
    #endif
}

MaynardLauncher::~MaynardLauncher()
{
    /*
  // These calls to Gtk::Widget::unparent() are necessary if MaynardLauncher is
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




void MaynardLauncher::maynard_launcher_calculate (MaynardLauncher *self,
    gint *grid_window_width,
    gint *grid_window_height,
    gint *grid_cols)
{
  gint output_width, output_height, panel_height;
  gint usable_width, usable_height;
  guint cols, rows;
  guint num_apps;
  guint scrollbar_width = 13;

  // gtk_widget_get_size_request (self->background,
  //     &output_width, &output_height);
  output_width = 1024;    //hyjiang
  output_height = 768;

  panel_height = output_height * MAYNARD_PANEL_HEIGHT_RATIO;

  usable_width = output_width - MAYNARD_PANEL_WIDTH - scrollbar_width;
  /* don't go further down than the panel */
  usable_height = panel_height - MAYNARD_CLOCK_HEIGHT;

  /* try and fill half the screen, otherwise round down */
  cols = (int) ((usable_width / 2.0) / GRID_ITEM_WIDTH);
  /* try to fit as many rows as possible in the panel height we have */
  rows = (int) (usable_height / GRID_ITEM_HEIGHT);

  /* we don't need to include the scrollbar if we already have enough
   * space for all the apps. */
  num_apps = g_hash_table_size (
      shell_app_system_get_entries (self->app_system));
  if ((cols * rows) >= num_apps)
    scrollbar_width = 0;

  /* done! */

  if (grid_window_width)
    *grid_window_width = (cols * GRID_ITEM_WIDTH) + scrollbar_width;

  if (grid_window_height)
    *grid_window_height = (rows * GRID_ITEM_HEIGHT);

  if (grid_cols)
    *grid_cols = cols;
}


static gboolean
get_child_position_cb (GtkOverlay *overlay,
    GtkWidget *widget,
    GdkRectangle *allocation,
    gpointer user_data)
{
  GtkOverlayClass *klass = GTK_OVERLAY_GET_CLASS (overlay);

  klass->get_child_position (overlay, widget, allocation);

  /* use the same valign and halign properties, but given we have a
   * border of 1px, respect it and don't draw the overlay widget over
   * the border. */
  allocation->x += 1;
  allocation->y -= 1;
  allocation->width -= 2;

  return TRUE;
}

static void
clicked_cb (GtkWidget *widget,
    GDesktopAppInfo *info)
{
  MaynardLauncher *self;

  g_app_info_launch (G_APP_INFO (info), NULL, NULL, NULL);

  self = (MaynardLauncher*)g_object_get_data (G_OBJECT (widget), "launcher");
  g_assert (self);
  // g_signal_emit (self, signals[APP_LAUNCHED], 0);

  /* do this in an idle so it's not done so obviously onscreen */
  // g_idle_add (app_launched_idle_cb, self);
}


static gboolean
app_enter_cb (GtkWidget *widget,
    GdkEvent *event,
    GtkWidget *revealer)
{
  gtk_revealer_set_reveal_child (GTK_REVEALER (revealer), TRUE);
  return FALSE;
}


static gboolean
app_leave_cb (GtkWidget *widget,
    GdkEvent *event,
    GtkWidget *revealer)
{
  gtk_revealer_set_reveal_child (GTK_REVEALER (revealer), FALSE);
  return FALSE;
}

GtkWidget* MaynardLauncher::app_launcher_new_from_desktop_info (MaynardLauncher *self,
    GDesktopAppInfo *info)
{
  GIcon *icon;
  GtkWidget *image;
  GtkWidget *button;
  GtkWidget *overlay;
  GtkWidget *revealer;
  GtkWidget *label;
  GtkWidget *ebox;

  /* we need an ebox to catch enter and leave events */
  ebox = gtk_event_box_new ();
  gtk_style_context_add_class (gtk_widget_get_style_context (ebox),
      "maynard-grid-item");

  /* we use an overlay so we can have the app icon showing but use a
   * GtkRevealer to show a label of the app's name. */
  overlay = gtk_overlay_new ();
  gtk_container_add (GTK_CONTAINER (ebox), overlay);

  /* ...but each item has a border of 1px and we don't want the
   * revealer to paint into the border, so overload this function to
   * know where to put it. */
  g_signal_connect (overlay, "get-child-position",
      G_CALLBACK (get_child_position_cb), NULL);

  revealer = gtk_revealer_new ();
  g_object_set (revealer,
      "halign", GTK_ALIGN_FILL, /* all the width */
      "valign", GTK_ALIGN_END, /* only at the bottom */
      NULL);
  gtk_revealer_set_transition_type (GTK_REVEALER (revealer),
      GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);
  gtk_revealer_set_reveal_child (GTK_REVEALER (revealer), FALSE);
  gtk_overlay_add_overlay (GTK_OVERLAY (overlay), revealer);

  /* app name */
  label = gtk_label_new (g_app_info_get_display_name (G_APP_INFO (info)));
  printf("app name = %s\n",gtk_label_get_label((GtkLabel*)label));
  gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);
  gtk_style_context_add_class (gtk_widget_get_style_context (label), "maynard-grid-label");
  gtk_container_add (GTK_CONTAINER (revealer), label);

  /* icon button to load the app */
  icon = g_app_info_get_icon (G_APP_INFO (info));
  image = gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_DIALOG);
  button = gtk_button_new ();
  gtk_style_context_remove_class (
      gtk_widget_get_style_context (button),
      "button");
  gtk_style_context_remove_class (
      gtk_widget_get_style_context (button),
      "image-button");
  gtk_button_set_image (GTK_BUTTON (button), image);
  g_object_set (image,
      "margin", 30,
      NULL);
  gtk_container_add (GTK_CONTAINER (overlay), button);

  /* TODO: a bit ugly */
  g_object_set_data (G_OBJECT (button), "launcher", self);
  g_signal_connect (button, "clicked", G_CALLBACK (clicked_cb), info);

  /* now we have set everything up, we can refernce the ebox and the
   * revealer. enter will show the label and leave will hide the label. */
  g_signal_connect (ebox, "enter-notify-event", G_CALLBACK (app_enter_cb), revealer);
  g_signal_connect (ebox, "leave-notify-event", G_CALLBACK (app_leave_cb), revealer);

  return ebox;
}


static gint
sort_apps (gconstpointer a,
    gconstpointer b)
{
  GAppInfo *info1 = G_APP_INFO (a);
  GAppInfo *info2 = G_APP_INFO (b);
  gchar *s1, *s2;
  gint ret;

  s1 = g_utf8_casefold (g_app_info_get_display_name (info1), -1);
  s2 = g_utf8_casefold (g_app_info_get_display_name (info2), -1);

  ret = g_strcmp0 (s1, s2);

  g_free (s1);
  g_free (s2);

  return ret;
}

void MaynardLauncher::installed_changed_cb(ShellAppSystem *app_system, MaynardLauncher* self)
{
    GHashTable *entries = shell_app_system_get_entries(app_system);
    GList *l, *values;

    gint output_width, output_height;
    gint cols;
    guint left, top;

    /* remove all children first */
    self->grid.foreach([&](Gtk::Widget& widget){
        self->grid.remove(widget);
    });

    values = g_hash_table_get_values(entries);
    values = g_list_sort(values, sort_apps);

    maynard_launcher_calculate(self, NULL, NULL, &cols);
    cols--; /* because we start from zero here */

    left = top = 0;
    for (l = values; l; l = l->next)
    {
        GDesktopAppInfo *info = G_DESKTOP_APP_INFO(l->data);
        GtkWidget *app = app_launcher_new_from_desktop_info(self, info);
        Gtk::Widget *app2 = Glib::wrap(app);

        self->grid.attach(*Gtk::manage(app2), left++, top, 1, 1);

        if (left > cols)
        {
            left = 0;
            top++;
        }
    }

    g_list_free(values);

    self->grid.show_all_children();
}

