#include "Panel.h"
#include <iostream>
#include "../shell/weston-desktop-shell-client-protocol.h"
#include "../shell/shell-helper-client-protocol.h"
#include "Wayland.h"

PanelWindow::PanelWindow()
    : main_box(Gtk::ORIENTATION_VERTICAL, 0),
      menu_box(Gtk::ORIENTATION_VERTICAL, 0),
      buttons_box(Gtk::ORIENTATION_VERTICAL, 0)
{
    set_title("Gtk::ColorChooserDialog example");
    set_default_size(20, 200);

    // /* make it black and slightly alpha */
    // gtk_style_context_add_class(
    //     gtk_widget_get_style_context(GTK_WIDGET(self)),
    //     "maynard-panel");

    /* main vbox */
    add(main_box);

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
    // gtk_box_pack_start(GTK_BOX(main_box), ebox, FALSE, FALSE, 0);
    Gtk::EventBox ebox;
    main_box.add(ebox);
    // widget_connect_enter_signal(self, ebox);
    ebox.add(menu_box);

    /* revealer for the top buttons */
    // gtk_revealer_set_transition_type(GTK_REVEALER(self->priv->revealer_buttons),
    //                                  GTK_REVEALER_TRANSITION_TYPE_SLIDE_LEFT);
    // gtk_revealer_set_reveal_child(GTK_REVEALER(self->priv->revealer_buttons),
    //                               TRUE);
    menu_box.add(revealer_buttons);

    /* the box for the top buttons */
    buttons_box.add(revealer_buttons);

    /* system button */
    Gtk::EventBox ebox2;
    buttons_box.add(ebox2);

    system_button.set_image_from_icon_name("emblem-system-symbolic", Gtk::ICON_SIZE_LARGE_TOOLBAR);
    // enum BuiltinIconSize
    // {
    //     ICON_SIZE_INVALID,
    //     ICON_SIZE_MENU,
    //     ICON_SIZE_SMALL_TOOLBAR,
    //     ICON_SIZE_LARGE_TOOLBAR,
    //     ICON_SIZE_BUTTON,
    //     ICON_SIZE_DND,
    //     ICON_SIZE_DIALOG
    // };
    // gtk_style_context_add_class(gtk_widget_get_style_context(button),
    //                             "maynard-system");
    // gtk_style_context_remove_class(gtk_widget_get_style_context(button),
    //                                "button");
    // gtk_style_context_remove_class(gtk_widget_get_style_context(button),
    //                                "image-button");
    // g_signal_connect(button, "clicked",
    //                  G_CALLBACK(system_button_clicked_cb), self);
    ebox2.add(system_button);
    // widget_connect_enter_signal(self, ebox);


    // /* favorites */
    // ebox = gtk_event_box_new();
    // gtk_box_pack_start(GTK_BOX(main_box), ebox, FALSE, FALSE, 0);
    // favorites = maynard_favorites_new();
    // gtk_container_add(GTK_CONTAINER(ebox), favorites);
    // widget_connect_enter_signal(self, ebox);

    // g_signal_connect(favorites, "app-launched",
    //                  G_CALLBACK(favorite_launched_cb), self);

    // /* bottom app menu button */
    // ebox = gtk_event_box_new();
    // gtk_box_pack_end(GTK_BOX(main_box), ebox, FALSE, FALSE, 0);
    // button = maynard_app_icon_new("view-grid-symbolic");
    // g_signal_connect(button, "clicked",
    //                  G_CALLBACK(app_menu_button_clicked_cb), self);
    // gtk_container_add(GTK_CONTAINER(ebox), button);
    // widget_connect_enter_signal(self, ebox);

    // /* done */
    // self->priv->hidden = FALSE;
    // self->priv->volume_showing = FALSE;





    /* system button */
    // GtkButton* button = (GtkButton*)gtk_button_new_from_icon_name("emblem-system-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);
    // gtk_style_context_add_class(gtk_widget_get_style_context(button),
    //                             "maynard-system");
    // gtk_style_context_remove_class(gtk_widget_get_style_context(button),
    //                                "button");
    // gtk_style_context_remove_class(gtk_widget_get_style_context(button),
    //                                "image-button");
    // g_signal_connect(button, "clicked",
    //                  G_CALLBACK(system_button_clicked_cb), self);
    // gtk_container_add(GTK_CONTAINER(ebox), button);
    // widget_connect_enter_signal(self, ebox);

    // Gtk::Button* tmp = Glib::wrap(button);
    // tmp->set
    // m_VBox.pack_start(*tmp);


    // m_VBox.pack_start(m_MenuBox);
    // m_VBox.pack_start(revealer_buttons);
    // m_VBox.pack_start(revealer_clock);
    // system_button.set_image_from_icon_name("emblem-system-symbolic");
    // system_button.set_size_request(20, 20);
    // m_VBox.pack_start(system_button);
    // m_VBox.pack_start(volume_button);

    // m_VBox.pack_start(m_ColorButton, Gtk::PACK_SHRINK);
    // m_ColorButton.signal_color_set().connect(sigc::mem_fun(*this,
    //                                                        &PanelWindow::on_color_button_color_set));

    // m_VBox.pack_start(m_Button_Dialog, Gtk::PACK_SHRINK);
    // m_Button_Dialog.signal_clicked().connect(sigc::mem_fun(*this,
    //                                                        &PanelWindow::on_button_dialog_clicked));

    //Set start color:
    // m_Color.set_red(0.0);
    // m_Color.set_green(0.0);
    // m_Color.set_blue(1.0);
    // m_Color.set_alpha(1.0); //opaque
    // m_ColorButton.set_rgba(m_Color);

    // m_VBox.pack_start(m_DrawingArea);
    // m_DrawingArea.signal_draw().connect(sigc::mem_fun(*this,
    //                                                   &PanelWindow::on_drawing_area_draw));

    Wayland::getInstance()->panel = this->window;
    weston_desktop_shell_set_panel(Wayland::getInstance()->wshell, Wayland::getInstance()->output,
                                        this->surface);
    weston_desktop_shell_set_panel_position(Wayland::getInstance()->wshell,
                                            WESTON_DESKTOP_SHELL_PANEL_POSITION_LEFT);

    show_all_children();
}

PanelWindow::~PanelWindow()
{
}

void PanelWindow::on_color_button_color_set()
{
    //Store the chosen color:
    // m_Color = m_ColorButton.get_rgba();
}

void PanelWindow::on_button_dialog_clicked()
{
    // Gtk::ColorChooserDialog dialog("Please choose a color");
    // dialog.set_transient_for(*this);

    // //Get the previously selected color:
    // dialog.set_rgba(m_Color);

    // const int result = dialog.run();

    // //Handle the response:
    // switch (result)
    // {
    // case Gtk::RESPONSE_OK:
    // {
    //     //Store the chosen color:
    //     m_Color = dialog.get_rgba();
    //     m_ColorButton.set_rgba(m_Color);
    //     break;
    // }
    // case Gtk::RESPONSE_CANCEL:
    // {
    //     std::cout << "Cancel clicked." << std::endl;
    //     break;
    // }
    // default:
    // {
    //     std::cout << "Unexpected button clicked: " << result << std::endl;
    //     break;
    // }
    // }
}

bool PanelWindow::on_drawing_area_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    // Gdk::Cairo::set_source_rgba(cr, m_Color);
    // cr->paint();

    return true;
}
