#include "Wayland.h"
#include "Config.h"

//----------------------------------------------------------------------------

static void
pointer_handle_enter(void *data,
                     struct wl_pointer *pointer,
                     uint32_t serial,
                     struct wl_surface *surface,
                     wl_fixed_t sx_w,
                     wl_fixed_t sy_w)
{
    // printf("%s[%d] pointer_handle_enter\n",__FILE__,__LINE__);
}

static void
pointer_handle_leave(void *data,
                     struct wl_pointer *pointer,
                     uint32_t serial,
                     struct wl_surface *surface)
{
    // printf("%s[%d] pointer_handle_leave\n",__FILE__,__LINE__);
}

static void
pointer_handle_motion(void *data,
                      struct wl_pointer *pointer,
                      uint32_t time,
                      wl_fixed_t sx_w,
                      wl_fixed_t sy_w)
{
    // printf("%s[%d] pointer_handle_motion\n",__FILE__,__LINE__);
}

static void
pointer_handle_button(void *data,
                      struct wl_pointer *pointer,
                      uint32_t serial,
                      uint32_t time,
                      uint32_t button,
                      uint32_t state_w)
{
    printf("%s[%d] pointer_handle_button\n",__FILE__,__LINE__);

    Wayland *desktop = (Wayland *)data;

    if (state_w != WL_POINTER_BUTTON_STATE_RELEASED)
        return;

    //   if (!desktop->pointer_out_of_panel)
    //     return;

    //   if (desktop->grid_visible)
    //     launcher_grid_toggle (desktop->launcher_grid->window, desktop);

    //   panel_window_leave_cb (NULL, NULL, desktop);
}

static void
pointer_handle_axis(void *data,
                    struct wl_pointer *pointer,
                    uint32_t time,
                    uint32_t axis,
                    wl_fixed_t value)
{
    // printf("%s[%d] pointer_handle_axis\n",__FILE__,__LINE__);
}

static const struct wl_pointer_listener pointer_listener = {
    pointer_handle_enter,
    pointer_handle_leave,
    pointer_handle_motion,
    pointer_handle_button,
    pointer_handle_axis,
};

static void
seat_handle_capabilities(void *data,
                         struct wl_seat *seat,
                         uint32_t caps)
{
    Wayland *desktop = (Wayland *)data;
    printf("%s[%d] seat_handle_capabilities\n",__FILE__,__LINE__);

    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !desktop->pointer)
    {
        desktop->pointer = wl_seat_get_pointer(seat);
        wl_pointer_set_user_data(desktop->pointer, desktop);
        wl_pointer_add_listener(desktop->pointer, &pointer_listener, desktop);
    }
    else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && desktop->pointer)
    {
        wl_pointer_destroy(desktop->pointer);
        desktop->pointer = NULL;
    }

    /* TODO: keyboard and touch */
    // if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !desktop->keyboard) {
    // 	desktop->keyboard = wl_seat_get_keyboard(seat);
    // 	wl_keyboard_set_user_data(desktop->keyboard, desktop);
    // 	wl_keyboard_add_listener(desktop->keyboard, &keyboard_listener,               //<------------------- hyjiang, setup keyboard listener
    // 				 desktop);
    // } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && desktop->keyboard) {
    // 	if (desktop->seat_version >= WL_KEYBOARD_RELEASE_SINCE_VERSION)
    // 		wl_keyboard_release(desktop->keyboard);
    // 	else
    // 		wl_keyboard_destroy(desktop->keyboard);
    // 	desktop->keyboard = NULL;
    // }
}

static void
seat_handle_name(void *data,
                 struct wl_seat *seat,
                 const char *name)
{
    printf("%s[%d] seat_handle_name, name=%s\n",__FILE__,__LINE__,name);
}

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
    seat_handle_name};

static void
shell_configure(Wayland *desktop,
                uint32_t edges,
                struct wl_surface *surface,
                int32_t width, int32_t height)
{
    int window_height;
    int grid_width, grid_height;
    gtk_widget_set_size_request(desktop->background.window, //hyjiang, adjust background size
                                width, height);

    /* TODO: make this height a little nicer */
    if(desktop->panel.window)
    {
        window_height = height * MAYNARD_PANEL_HEIGHT_RATIO;
        gtk_window_resize(GTK_WINDOW(desktop->panel.window), //hyjiang, adjust panel size
                        MAYNARD_PANEL_WIDTH, window_height);
    }

    if(desktop->launcher_grid.window)
    {
        //   maynard_launcher_calculate (MAYNARD_LAUNCHER (desktop->launcher_grid.window),
        //       &grid_width, &grid_height, NULL);
        grid_width = 60;
        grid_height = 1000;
        gtk_widget_set_size_request(desktop->launcher_grid.window, //hyjiang, adjust lancher_grid size
                                    grid_width, grid_height);
    }

    if(desktop->panel.surface)
    {
        shell_helper_move_surface(desktop->helper, desktop->panel.surface,
                                0, (height - window_height) / 2);
    }

    //   gtk_window_resize (GTK_WINDOW (desktop->clock->window),             //hyjiang, adjust clock size
    //       MAYNARD_CLOCK_WIDTH, MAYNARD_CLOCK_HEIGHT);

    //   shell_helper_move_surface (desktop->helper, desktop->clock->surface,
    //       MAYNARD_PANEL_WIDTH, (height - window_height) / 2);

    if(desktop->launcher_grid.surface)
    {
        shell_helper_move_surface(desktop->helper,
                                desktop->launcher_grid.surface,
                                -grid_width,
                                ((height - window_height) / 2) + MAYNARD_CLOCK_HEIGHT);
    }

    // weston_desktop_shell_desktop_ready(desktop->wshell);

    /* TODO: why does the panel signal leave on drawing for
   * startup? we don't want to have to have this silly
   * timeout. */
    //   g_timeout_add_seconds (1, connect_enter_leave_signals, desktop);
}

static void
weston_desktop_shell_configure(void *data,
                               struct weston_desktop_shell *weston_desktop_shell,
                               uint32_t edges,
                               struct wl_surface *surface,
                               int32_t width, int32_t height)
{
    printf("%s[%d] weston_desktop_shell_configure, %d %d %d\n",__FILE__,__LINE__,width,height,edges);
    shell_configure((Wayland *)data, edges, surface, width, height);
}

static void
weston_desktop_shell_prepare_lock_surface(void *data,
                                          struct weston_desktop_shell *weston_desktop_shell)
{
    weston_desktop_shell_unlock(weston_desktop_shell);
    printf("%s[%d] weston_desktop_shell_prepare_lock_surface\n",__FILE__,__LINE__);
}

static void
weston_desktop_shell_grab_cursor(void *data,
                                 struct weston_desktop_shell *weston_desktop_shell,
                                 uint32_t cursor)
{
    printf("%s[%d] weston_desktop_shell_grab_cursor\n",__FILE__,__LINE__);
}

static const struct weston_desktop_shell_listener wshell_listener = {
    weston_desktop_shell_configure,
    weston_desktop_shell_prepare_lock_surface,
    weston_desktop_shell_grab_cursor};



static void
output_handle_geometry(void *data,
                       struct wl_output *wl_output,
                       int x, int y,
                       int physical_width,
                       int physical_height,
                       int subpixel,
                       const char *make,
                       const char *model,
                       int transform)
{
    printf("%s[%d] output_handle_geometry, %d %d %d %d %d %s %s %d\n",
    __FILE__,__LINE__,x,y,physical_width,physical_height,subpixel,make,model,transform);

	// struct output *output = data;

	// output->x = x;
	// output->y = y;

	// if (output->panel)
	// 	window_set_buffer_transform(output->panel->window, transform);
	// if (output->background)
	// 	window_set_buffer_transform(output->background->window, transform);
}

static void
output_handle_mode(void *data,
		   struct wl_output *wl_output,
		   uint32_t flags,
		   int width,
		   int height,
		   int refresh)
{
    printf("%s[%d] output_handle_mode, %d %d %d %d\n",__FILE__,__LINE__,width,height,refresh,flags);

}

static void
output_handle_done(void *data,
                   struct wl_output *wl_output)
{
    printf("%s[%d] output_handle_done\n",__FILE__,__LINE__);
}

static void
output_handle_scale(void *data,
                    struct wl_output *wl_output,
                    int32_t scale)
{
    printf("%s[%d] output_handle_scale, scale = %d\n",__FILE__,__LINE__,scale);

	// struct output *output = data;

	// if (output->panel)
	// 	window_set_buffer_scale(output->panel->window, scale);
	// if (output->background)
	// 	window_set_buffer_scale(output->background->window, scale);
}

static const struct wl_output_listener output_listener = {
	output_handle_geometry,
	output_handle_mode,
	output_handle_done,
	output_handle_scale
};

// static void
// output_init(struct output *output, struct desktop *desktop)
// {
// 	struct wl_surface *surface;

// 	if (desktop->want_panel) {
// 		output->panel = panel_create(desktop, output);
// 		surface = window_get_wl_surface(output->panel->window);
// 		weston_desktop_shell_set_panel(desktop->shell,
// 					       output->output, surface);
// 	}

// 	output->background = background_create(desktop, output);
// 	surface = window_get_wl_surface(output->background->window);
// 	weston_desktop_shell_set_background(desktop->shell,
// 					    output->output, surface);
// }

static void
create_output(Wayland *d, uint32_t id)
{
	struct output *output;

	output = (struct output*)malloc(sizeof(output));
	if (!output)
		return;

	output->output =
		(wl_output*)wl_registry_bind(d->registry, id, &wl_output_interface, 2);
	output->server_output_id = id;
    d->output = output->output;

	wl_output_add_listener(output->output, &output_listener, output);

	wl_list_insert(&d->outputs, &output->link);

	/* On start up we may process an output global before the shell global
	 * in which case we can't create the panel and background just yet */
	// if (d->shell)
	// 	output_init(output, desktop);
}


static void
registry_handle_global(void *data,
                       struct wl_registry *registry,
                       uint32_t name,
                       const char *interface,
                       uint32_t version)
{
    Wayland *d = (Wayland *)data;
    printf("%s[%d] interface = %s %d %d\n",__FILE__,__LINE__,interface,name,version);
    if (!strcmp(interface, "weston_desktop_shell"))
    {
        d->wshell = (weston_desktop_shell *)wl_registry_bind(registry, name,
                                                             &weston_desktop_shell_interface, MIN(version, 1));
        weston_desktop_shell_add_listener(d->wshell, &wshell_listener, d);
        weston_desktop_shell_set_user_data(d->wshell, d);
    }
    else if (!strcmp(interface, "wl_output"))
    {
        /* TODO: create multiple outputs */
        // d->output = (wl_output *)wl_registry_bind(registry, name,
        //                                           &wl_output_interface, 1);
        create_output(d, name);
    }
    else if (!strcmp(interface, "wl_seat"))
    {
        //   d->seat_version = MIN(version, 7);
        d->seat = (wl_seat *)wl_registry_bind(registry, name,
                                              &wl_seat_interface, 1);
        wl_seat_add_listener(d->seat, &seat_listener, d);
    }
    else if (!strcmp(interface, "shell_helper"))
    {
        d->helper = (shell_helper *)wl_registry_bind(registry, name,
                                                     &shell_helper_interface, 1);
    }
}

static void
registry_handle_global_remove(void *data,
                              struct wl_registry *registry,
                              uint32_t name)
{
    printf("%s[%d] registry_handle_global_remove, name=%d\n",__FILE__,__LINE__, name);
    // Wayland *d = (Wayland *)data;
	// struct global *global;
	// struct global *tmp;

	// wl_list_for_each_safe(global, tmp, &d->global_list, link) {
	// 	if (global->name != name)
	// 		continue;

	// 	if (strcmp(global->interface, "wl_output") == 0)
	// 		display_destroy_output(d, name);

	// 	/* XXX: Should destroy remaining bound globals */

	// 	if (d->global_handler_remove)
	// 		d->global_handler_remove(d, name, global->interface,
	// 				global->version, d->user_data);

	// 	wl_list_remove(&global->link);
	// 	free(global->interface);
	// 	free(global);
	// }
}



static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove};

//----------------------------------------------------------------------------
/* Null, because instance will be initialized on demand. */
Wayland *Wayland::instance = 0;

Wayland *Wayland::getInstance()
{
    if (instance == 0)
    {
        instance = new Wayland();
    }

    return instance;
}

Wayland::Wayland()
{
    initial_panel_timeout_id = 0;
    hide_panel_idle_id = 0;

    grid_visible = false;
    system_visible = false;
    volume_visible = false;
    pointer_out_of_panel = false;

    display = NULL;
    registry = NULL;
    wshell = NULL;
    output = NULL;
    helper = NULL;
    gdk_display = NULL;
    seat = NULL;
    pointer = NULL;

	wl_list_init(&outputs);

    this->gdk_display = gdk_display_get_default();
    this->display = gdk_wayland_display_get_wl_display(this->gdk_display);
    if (this->display == NULL)
    {
        fprintf(stderr, "failed to get display: %m\n");
        return;
    }
    this->registry = wl_display_get_registry(this->display);
    wl_registry_add_listener(this->registry,
                             &registry_listener, this);
}

void Wayland::desktop_ready()
{
    weston_desktop_shell_desktop_ready(this->wshell);
}

void Wayland::launcher_grid_toggle()
{
    if (this->grid_visible)
    {
        shell_helper_slide_surface_back(this->helper,
                                        this->launcher_grid.surface);

        // shell_helper_curtain(this->helper, this->curtain->surface, 0);
    }
    else
    {
        int width;

        gtk_widget_get_size_request(this->launcher_grid.window,
                                    &width, NULL);

        shell_helper_slide_surface(this->helper,
                                   this->launcher_grid.surface,
                                   width + MAYNARD_PANEL_WIDTH, 0);

        // shell_helper_curtain(this->helper, this->curtain->surface, 1);
    }

    this->grid_visible = !this->grid_visible;
}

void Wayland::system_toggled()
{
}

void Wayland::volume_toggled()
{
}

void Wayland::favorite_launched()
{
}

void Wayland::button_toggled_cb(
    gboolean *visible,
    gboolean *not_visible)
{
    *visible = !*visible;
    *not_visible = FALSE;

    // if (this->system_visible)
    // {
    //     maynard_clock_show_section(MAYNARD_CLOCK(this->clock.window),
    //                                MAYNARD_CLOCK_SECTION_SYSTEM);
    //     maynard_panel_show_previous(MAYNARD_PANEL(this->panel.window),
    //                                 MAYNARD_PANEL_BUTTON_SYSTEM);
    // }
    // else if (this->volume_visible)
    // {
    //     maynard_clock_show_section(MAYNARD_CLOCK(this->clock.window),
    //                                MAYNARD_CLOCK_SECTION_VOLUME);
    //     maynard_panel_show_previous(MAYNARD_PANEL(this->panel.window),
    //                                 MAYNARD_PANEL_BUTTON_VOLUME);
    // }
    // else
    // {
    //     maynard_clock_show_section(MAYNARD_CLOCK(this->clock.window),
    //                                MAYNARD_CLOCK_SECTION_CLOCK);
    //     maynard_panel_show_previous(MAYNARD_PANEL(this->panel.window),
    //                                 MAYNARD_PANEL_BUTTON_NONE);
    // }
}
