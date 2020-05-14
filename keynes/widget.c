/* widget.c */

#include "widget.h"

/* Private data structure */
struct _MyWidgetPrivate {
   GdkWindow *window;

  /* instance members */
  guint desktop_width;
  guint desktop_height;
     
};

/* Internal API */
static void my_cpu_size_allocate(GtkWidget *widget, 
    GtkAllocation *allocation);
static void my_cpu_realize(GtkWidget *widget);
static gboolean my_cpu_expose(GtkWidget *widget, 
    cairo_t          *cr);

static void my_cpu_get_preferred_height(GtkWidget *widget,
                                        gint *minimum_height,
                                        gint *natural_height);
static void my_cpu_get_preferred_width(GtkWidget *widget,
                                       gint *minimum_width,
                                       gint *natural_width);

/* Define type */
G_DEFINE_TYPE(MyWidget, my_widget, GTK_TYPE_WIDGET)

enum
{
   PROP_0 = 0,   
   PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH,
   PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT,
   PROP_XA_WM_CHANGE_STATE,
   PROP_XA_WM_CLASS,
   PROP_XA_WM_CLIENT_LEADER,
   PROP_XA_WM_CLIENT_MACHINE,
   PROP_XA_WM_COLORMAP_NOTIFY,
   PROP_XA_WM_COLORMAP_WINDOWS,
   PROP_XA_WM_COMMAND,
   PROP_XA_WM_DELETE_WINDOW,
   PROP_XA_WM_DESKTOP,
   PROP_XA_WM_HINTS,
   PROP_XA_WM_ICON_NAME,
   PROP_XA_WM_ICON_SIZE,
   PROP_XA_WM_LOCALE_NAME,
   PROP_XA_WM_NAME,
   PROP_XA_WM_NORMAL_HINTS,
   PROP_XA_WM_PROTOCOLS,
   PROP_XA_WM_SIZE_HINTS,
   PROP_XA_WM_STATE,
   PROP_XA_WM_TAKE_FOCUS,
   PROP_XA_WM_TRANSIENT_FOR,
   PROP_XA_WM_WINDOW_ROLE,
   PROP_XA_WM_ZOOM_HINTS,

   PROP_XATOM_MWM_HINTS,
   //PROP_XA_MOTIF_WM_INFO,!!!
   PROP_XA_WINDOW_ROLE,
   PROP_XA_SM_CLIENT_ID,
   PROP_XA_ICEWM_ACTION,
   PROP_XA_ICEWM_GUIEVENT,
   PROP_XA_ICEWM_HINT,
   PROP_XA_ICEWM_FONT_PATH,
   PROP_XA_ICEWMBG_IMAGE,
   PROP_XA_XROOTPMAP_ID,
   PROP_XA_XROOTCOLOR_PIXEL,
   PROP_XA_GDK_TIMESTAMP_PROP,
   PROP_XA_CLIPBOARD,
   PROP_XA_MANAGER,
   PROP_XA_TARGETS,
   PROP_XA_XEMBED,
   PROP_XA_XEMBED_INFO,
   PROP_XA_UTF8_STRING,

   PROP_XA_WIN_APP_STATE,
   PROP_XA_WIN_AREA_COUNT,
   PROP_XA_WIN_AREA,
   PROP_XA_WIN_CLIENT_LIST,
   PROP_XA_WIN_DESKTOP_BUTTON_PROXY,
   PROP_XA_WIN_EXPANDED_SIZE,
   PROP_XA_WIN_HINTS,
   PROP_XA_WIN_ICONS,
   PROP_XA_WIN_LAYER,
   PROP_XA_WIN_PROTOCOLS,
   PROP_XA_WIN_STATE,
   PROP_XA_WIN_SUPPORTING_WM_CHECK,
   PROP_XA_WIN_TRAY,
   PROP_XA_WIN_WORKAREA,
   PROP_XA_WIN_WORKSPACE_COUNT,
   PROP_XA_WIN_WORKSPACE_NAMES,
   PROP_XA_WIN_WORKSPACE,

   PROP_XA_NET_ACTIVE_WINDOW,
   PROP_XA_NET_CLIENT_LIST,
   PROP_XA_NET_CLIENT_LIST_STACKING,
   PROP_XA_NET_CLOSE_WINDOW,
   PROP_XA_NET_CURRENT_DESKTOP,
   //PROP_XA_NET_DESKTOP_GEOMETRY,
   // PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH,
   // PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT,
   PROP_XA_NET_DESKTOP_LAYOUT,
   PROP_XA_NET_DESKTOP_NAMES,
   PROP_XA_NET_DESKTOP_VIEWPORT,
   PROP_XA_NET_FRAME_EXTENTS,
   PROP_XA_NET_MOVERESIZE_WINDOW,
   PROP_XA_NET_NUMBER_OF_DESKTOPS,
   PROP_XA_NET_PROPERTIES,
   PROP_XA_NET_REQUEST_FRAME_EXTENTS,
   PROP_XA_NET_RESTACK_WINDOW,
   PROP_XA_NET_SHOWING_DESKTOP,
   PROP_XA_NET_STARTUP_ID,
   PROP_XA_NET_STARTUP_INFO_BEGIN,
   PROP_XA_NET_STARTUP_INFO,
   PROP_XA_NET_SUPPORTED,
   PROP_XA_NET_SUPPORTING_WM_CHECK,
   PROP_XA_NET_SYSTEM_TRAY_MESSAGE_DATA,
   PROP_XA_NET_SYSTEM_TRAY_OPCODE,
   PROP_XA_NET_SYSTEM_TRAY_ORIENTATION,
   PROP_XA_NET_SYSTEM_TRAY_VISUAL,
   PROP_XA_NET_VIRTUAL_ROOTS,
   PROP_XA_NET_WM_ACTION_ABOVE,
   PROP_XA_NET_WM_ACTION_BELOW,
   PROP_XA_NET_WM_ACTION_CHANGE_DESKTOP,
   PROP_XA_NET_WM_ACTION_CLOSE,
   PROP_XA_NET_WM_ACTION_FULLSCREEN,
   PROP_XA_NET_WM_ACTION_MAXIMIZE_HORZ,
   PROP_XA_NET_WM_ACTION_MAXIMIZE_VERT,
   PROP_XA_NET_WM_ACTION_MINIMIZE,
   PROP_XA_NET_WM_ACTION_MOVE,
   PROP_XA_NET_WM_ACTION_RESIZE,
   PROP_XA_NET_WM_ACTION_SHADE,
   PROP_XA_NET_WM_ACTION_STICK,
   PROP_XA_NET_WM_ALLOWED_ACTIONS,
   PROP_XA_NET_WM_BYPASS_COMPOSITOR,
   PROP_XA_NET_WM_DESKTOP,
   PROP_XA_NET_WM_FULL_PLACEMENT,
   PROP_XA_NET_WM_FULLSCREEN_MONITORS,
   PROP_XA_NET_WM_HANDLED_ICONS,
   PROP_XA_NET_WM_ICON_GEOMETRY,
   PROP_XA_NET_WM_ICON_NAME,
   PROP_XA_NET_WM_ICON,
   PROP_XA_NET_WM_MOVERESIZE,
   PROP_XA_NET_WM_NAME,
   PROP_XA_NET_WM_OPAQUE_REGION,
   PROP_XA_NET_WM_PID,
   PROP_XA_NET_WM_PING,
   PROP_XA_NET_WM_STATE,
   PROP_XA_NET_WM_STATE_ABOVE,
   PROP_XA_NET_WM_STATE_BELOW,
   PROP_XA_NET_WM_STATE_DEMANDS_ATTENTION,
   PROP_XA_NET_WM_STATE_FOCUSED,
   PROP_XA_NET_WM_STATE_FULLSCREEN,
   PROP_XA_NET_WM_STATE_HIDDEN,
   PROP_XA_NET_WM_STATE_MAXIMIZED_HORZ,
   PROP_XA_NET_WM_STATE_MAXIMIZED_VERT,
   PROP_XA_NET_WM_STATE_MODAL,
   PROP_XA_NET_WM_STATE_SHADED,
   PROP_XA_NET_WM_STATE_SKIP_PAGER,
   PROP_XA_NET_WM_STATE_SKIP_TASKBAR,
   PROP_XA_NET_WM_STATE_STICKY,
   PROP_XA_NET_WM_STRUT,
   PROP_XA_NET_WM_STRUT_PARTIAL,
   PROP_XA_NET_WM_SYNC_REQUEST,
   PROP_XA_NET_WM_SYNC_REQUEST_COUNTER,
   PROP_XA_NET_WM_USER_TIME,
   PROP_XA_NET_WM_USER_TIME_WINDOW,
   PROP_XA_NET_WM_VISIBLE_ICON_NAME,
   PROP_XA_NET_WM_VISIBLE_NAME,
   PROP_XA_NET_WM_WINDOW_OPACITY,
   PROP_XA_NET_WM_WINDOW_TYPE,
   PROP_XA_NET_WM_WINDOW_TYPE_COMBO,
   PROP_XA_NET_WM_WINDOW_TYPE_DESKTOP,
   PROP_XA_NET_WM_WINDOW_TYPE_DIALOG,
   PROP_XA_NET_WM_WINDOW_TYPE_DND,
   PROP_XA_NET_WM_WINDOW_TYPE_DOCK,
   PROP_XA_NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
   PROP_XA_NET_WM_WINDOW_TYPE_MENU,
   PROP_XA_NET_WM_WINDOW_TYPE_NORMAL,
   PROP_XA_NET_WM_WINDOW_TYPE_NOTIFICATION,
   PROP_XA_NET_WM_WINDOW_TYPE_POPUP_MENU,
   PROP_XA_NET_WM_WINDOW_TYPE_SPLASH,
   PROP_XA_NET_WM_WINDOW_TYPE_TOOLBAR,
   PROP_XA_NET_WM_WINDOW_TYPE_TOOLTIP,
   PROP_XA_NET_WM_WINDOW_TYPE_UTILITY,
   PROP_XA_NET_WORKAREA,

   PROP_XA_KWM_DOCKWINDOW,
   PROP_XA_KWM_WIN_ICON,

   PROP_XA_KDE_NET_SYSTEM_TRAY_WINDOWS,
   PROP_XA_KDE_NET_WM_FRAME_STRUT,
   PROP_XA_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR,
   PROP_XA_KDE_NET_WM_WINDOW_TYPE_OVERRIDE,
   PROP_XA_KDE_SPLASH_PROGRESS,
   PROP_XA_KDE_WM_CHANGE_STATE,

   PROP_XA_XdndAware,
   PROP_XA_XdndDrop,
   PROP_XA_XdndEnter,
   PROP_XA_XdndFinished,
   PROP_XA_XdndLeave,
   PROP_XA_XdndPosition,
   PROP_XA_XdndProxy,
   PROP_XA_XdndStatus,

   N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };
// static GParamSpec *obj_properties[3] = { NULL, };

static void
widget_set_property(GObject *object,
                    guint property_id,
                    const GValue *value,
                    GParamSpec *pspec)
{
   MyWidget *self = MY_WIDGET(object);

   switch (property_id)
   {
   // case PROP_FILENAME:
   //    g_free(self->filename);
   //    self->filename = g_value_dup_string(value);
   //    g_print("filename: %s\n", self->filename);
   //    break;

   case PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH:
      self->priv->desktop_width = g_value_get_uint(value);
      g_print("desktop_width: %u\n", self->priv->desktop_width);
      break;

   case PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT:
      self->priv->desktop_height = g_value_get_uint(value);
      g_print("desktop_height: %u\n", self->priv->desktop_height);
      break;

   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
   }
}

static void
widget_get_property(GObject *object,
                    guint property_id,
                    GValue *value,
                    GParamSpec *pspec)
{
   MyWidget *self = MY_WIDGET(object);

   switch (property_id)
   {
   // case PROP_FILENAME:
   //    g_value_set_string(value, self->filename);
   //    break;

   case PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH:
      g_value_set_uint(value, self->priv->desktop_width);
      break;

   case PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT:
      g_value_set_uint(value, self->priv->desktop_height);
      break;

   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
      break;
   }
}

static void _init_properties(GObjectClass *g_class)
{
  g_class->set_property = widget_set_property;
  g_class->get_property = widget_get_property;

//   obj_properties[PROP_XA_NET_WM_STRUT] =
//     g_param_spec_string ("filename",
//                          "Filename",
//                          "Name of the file to load and display from.",
//                          NULL  /* default value */,
//                          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

  obj_properties[PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH] =
    g_param_spec_uint ("PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH",
                       "PROP_XA_NET_DESKTOP_GEOMETRY_WIDTH",
                       "DESKTOP_GEOMETRY_WIDTH",
                       0  /* minimum value */,
                       4000 /* maximum value */,
                       1  /* default value */,
                       G_PARAM_READWRITE);

  obj_properties[PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT] =
    g_param_spec_uint ("PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT",
                       "PROP_XA_NET_DESKTOP_GEOMETRY_HEIGHT",
                       "DESKTOP_GEOMETRY_HEIGHT",
                       0  /* minimum value */,
                       4000 /* maximum value */,
                       1  /* default value */,
                       G_PARAM_READWRITE);

  g_object_class_install_properties (g_class,
                                     N_PROPERTIES,
                                     obj_properties);
}



/* Initialization */
static void my_widget_class_init(MyWidgetClass *klass) {
    
   GObjectClass *g_class;
   GtkWidgetClass *w_class;
   GParamSpec *pspec;

   g_class = G_OBJECT_CLASS(klass);
   w_class = GTK_WIDGET_CLASS(klass);

   w_class->realize       = my_cpu_realize;
   // w_class->get_preferred_width  = my_cpu_get_preferred_width;
   // w_class->get_preferred_height  = my_cpu_get_preferred_height;
   // w_class->size_allocate = my_cpu_size_allocate;
   // w_class->scroll_event  = my_cpu_expose;
   w_class->draw = my_cpu_expose;
   

   /* Install property */
   // pspec = g_param_spec_double("percent", "Percent", 
   //     "What CPU load should be displayed", 0, 1, 0, 
   //     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
       
   // g_object_class_install_property(g_class, P_PERCENT, pspec);

   _init_properties(g_class);

   /* Add private data */
   g_type_class_add_private(g_class, sizeof(MyWidgetPrivate));
}

static void my_widget_init(MyWidget *cpu) {
    
   MyWidgetPrivate *priv;
   
   priv = G_TYPE_INSTANCE_GET_PRIVATE(cpu, MY_TYPE_WIDGET, MyWidgetPrivate);

   gtk_widget_set_has_window(GTK_WIDGET(cpu), TRUE);

   /* Set default values */
   // priv->percent = 0;

   /* Create cache for faster access */
   cpu->priv = priv;
}

/* Overriden virtual methods */
static void my_cpu_set_property(GObject *object, guint prop_id,
    const GValue *value, GParamSpec *pspec) {
        
   // MyWidget *cpu = MY_WIDGET(object);

   // switch(prop_id) {
       
   //    case P_PERCENT:
      
   //       my_cpu_set_percent(cpu, g_value_get_double(value));
   //       break;

   //    default:
      
   //       G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   //       break;
   // }
}

static void my_cpu_get_property(GObject *object, guint prop_id,
                GValue *value, GParamSpec *pspec) {
                    
   MyWidget *cpu = MY_WIDGET(object);

   // switch(prop_id) {
       
   //    case P_PERCENT:
   //       g_value_set_double(value, cpu->priv->percent);
   //       break;

   //    default:
   //       G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
   //       break;
   // }
}

static void my_cpu_realize(GtkWidget *widget) {
    
   MyWidgetPrivate *priv = MY_WIDGET(widget)->priv;
   GtkAllocation alloc;
   GdkWindowAttr attrs;
   guint attrs_mask;

   gtk_widget_set_realized(widget, TRUE);

   gtk_widget_get_allocation(widget, &alloc);

   attrs.x           = alloc.x;
   attrs.y           = alloc.y;
   attrs.width       = alloc.width;
   attrs.height      = alloc.height;
   attrs.window_type = GDK_WINDOW_CHILD;
   attrs.wclass      = GDK_INPUT_OUTPUT;
   attrs.event_mask  = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK;

   attrs_mask = GDK_WA_X | GDK_WA_Y;

   priv->window = gdk_window_new(gtk_widget_get_parent_window(widget),
               &attrs, attrs_mask);
   gdk_window_set_user_data(priv->window, widget);
   gtk_widget_set_window(widget, priv->window);

   // widget->style = gtk_style_attach(gtk_widget_get_style( widget ),
   //                           priv->window);
   // gtk_style_set_background(widget->style, priv->window, GTK_STATE_NORMAL);
}

static void my_cpu_size_allocate(GtkWidget *widget,
                 GtkAllocation *allocation) {
                     
   MyWidgetPrivate *priv;

   priv = MY_WIDGET(widget)->priv;

   gtk_widget_set_allocation(widget, allocation);

   if (gtk_widget_get_realized(widget)) {
       
      gdk_window_move_resize(priv->window, allocation->x, allocation->y,
          allocation->width, allocation->height);
   }
}


static gboolean my_cpu_expose(GtkWidget *widget, 
    cairo_t          *cr) {
                
   MyWidgetPrivate *priv = MY_WIDGET(widget)->priv;
   gint limit;
   gint i;

   cairo_translate(cr, 0, 7);

   cairo_set_source_rgb(cr, 0, 0, 0);
   cairo_paint(cr);

   // limit = 20 - priv->percent / 5;
   
   for (i = 1; i <= 20; i++) {
       
      if (i > limit) {
         cairo_set_source_rgb(cr, 0.6, 1.0, 0);
      } else {
         cairo_set_source_rgb(cr, 0.2, 0.4, 0);
      }

      cairo_rectangle(cr, 8,  i * 4, 30, 3);
      cairo_rectangle(cr, 42, i * 4, 30, 3);
      cairo_fill(cr);
   }

   return TRUE;
}

/* Public API */
GtkWidget *my_widget_new(void) {
    
   return(g_object_new(MY_TYPE_WIDGET, NULL));
}


guint my_widget_get_desktop_width(MyWidget *w)
{
   g_return_val_if_fail(MY_IS_WIDGET(w), 0);

   return(w->priv->desktop_width);
}

void  my_widget_set_desktop_width(MyWidget *w, guint width)
{
   g_return_if_fail(MY_IS_WIDGET(w));

   w->priv->desktop_width = width;
}

void my_widget_property_set_guint(GtkWidget *w, const char* property, guint value)
{
   g_return_if_fail(MY_IS_WIDGET(w));

   GValue val = G_VALUE_INIT;
   g_value_init(&val, G_TYPE_UINT);

   g_value_set_uint (&val, value);
   g_object_set_property(G_OBJECT(w), property, &val);

   g_value_unset(&val);
}
