#include <gdk/gdk.h>
#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>

#include "eggaccelerators.h"
#include "gkey-binding.h"
/* lot of code in there ripped from libtomboy */

static guint num_lock_mask, caps_lock_mask, scroll_lock_mask;
static gboolean processing_event = FALSE;
static guint32 last_event_time = 0;
static GSList *bindings = NULL;

static void gkey_binding_class_init(gpointer g_class, gpointer g_class_data);
static GdkFilterReturn gkey_binding_filter_func(GdkXEvent *gdk_xevent,
		GdkEvent *event, gpointer data);
static void gkey_binding_keymap_changed(GdkKeymap *map);
static void gkey_binding_grab_ungrab(GkeyBinding *gb, GdkWindow *rootwin,
		gboolean grab);

GType gkey_binding_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (GkeyBindingClass),
			NULL,
			NULL,
			gkey_binding_class_init,
			NULL,
			NULL,
			sizeof (GkeyBinding),
			0,
			NULL,
			NULL
		};
		type = g_type_register_static(G_TYPE_OBJECT, "GkeyBindingType",
				&info, 0);
	}
	return type;
}

static void gkey_binding_lookup_ignorable_modifiers(GdkKeymap *keymap);

static void gkey_binding_class_init(gpointer g_class, gpointer g_class_data)
{
	GkeyBindingClass *klass = GKEY_BINDING_CLASS(g_class);

	klass->pressed_signal_id = g_signal_newv("pressed",
		G_TYPE_FROM_CLASS(g_class),
		G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
		NULL /* class closure */,
		NULL /* accumulator */,
		NULL /* accu_data */,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE /* return_type */,
		0     /* n_params */,
		NULL  /* param_types */);

	(void)g_class_data;
}

GkeyBinding *gkey_binding_new()
{
	GkeyBinding *gb = GKEY_BINDING(g_object_new(GKEY_TYPE_BINDING, NULL));

	gb->keystring = NULL;
	gb->grabbed = FALSE;

	return gb;
}

/*
 * Maybe call this function at class initialization time
 */

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

void gkey_binding_system_init(void)
{
	GdkKeymap *keymap = gdk_keymap_get_default();
	GdkWindow *rootwin = gdk_get_default_root_window();

	gkey_binding_lookup_ignorable_modifiers(keymap);

	gdk_window_add_filter(rootwin, gkey_binding_filter_func, NULL);

	g_signal_connect(keymap, "keys_changed",
			G_CALLBACK(gkey_binding_keymap_changed), NULL);

	/* hack: do one wnck call, else the thingy does not work on first key
	 * stroke */
	wnck_screen_get(0);
}

/*
 * public instance methods
 */

void gkey_binding_ungrab(GkeyBinding *gb)
{
	GdkWindow *rootwin = gdk_get_default_root_window();
	gkey_binding_grab_ungrab(gb, rootwin, FALSE /* ungrab */);
	gb->grabbed = FALSE;
	bindings = g_slist_remove(bindings, gb);
}

gint gkey_binding_grab(GkeyBinding *gb, gchar *key)
{
	GdkKeymap *keymap;
	GdkWindow *rootwin;

	EggVirtualModifierType virtual_mods = 0;
	guint keysym = 0;

	if (gb->grabbed)
		gkey_binding_ungrab(gb);

	keymap = gdk_keymap_get_default();
	rootwin = gdk_get_default_root_window();

	if (keymap == NULL || rootwin == NULL)
		return FALSE;

	if (!egg_accelerator_parse_virtual(key, &keysym, &virtual_mods))
		return FALSE;

	gb->keycode = XKeysymToKeycode(GDK_WINDOW_XDISPLAY(rootwin), keysym);

	if (gb->keycode == 0)
		return FALSE;

	egg_keymap_resolve_virtual_modifiers(keymap, virtual_mods,
			&gb->modifiers);

	gdk_error_trap_push();

	gkey_binding_grab_ungrab(gb, rootwin, TRUE /* grab */);

	gdk_flush();
	if (gdk_error_trap_pop()) {
		g_warning ("Binding '%s' failed!\n", key);
		return FALSE;
	}

	gb->grabbed = TRUE;
	if (gb->keystring)
		g_free(gb->keystring);
	gb->keystring = g_strdup(key);
	bindings = g_slist_append(bindings, gb);
	return TRUE;
}

/*
 * Private functions
 */
static void gkey_binding_grab_ungrab(GkeyBinding *gb, GdkWindow *rootwin,
		gboolean grab)
{
	guint i;
	guint mod_masks [] = {
		0, /* modifier only */
		num_lock_mask,
		caps_lock_mask,
		scroll_lock_mask,
		num_lock_mask  | caps_lock_mask,
		num_lock_mask  | scroll_lock_mask,
		caps_lock_mask | scroll_lock_mask,
		num_lock_mask  | caps_lock_mask | scroll_lock_mask,
	};

	for (i = 0; i < G_N_ELEMENTS(mod_masks); i++) {
		if (grab) {
			XGrabKey(GDK_WINDOW_XDISPLAY (rootwin),
					gb->keycode,
					gb->modifiers | mod_masks[i],
					GDK_WINDOW_XWINDOW(rootwin), False,
					GrabModeAsync, GrabModeAsync);
		} else {
			XUngrabKey (GDK_WINDOW_XDISPLAY(rootwin),
					gb->keycode,
					gb->modifiers | mod_masks[i],
					GDK_WINDOW_XWINDOW(rootwin));
		}
	}
}

static void gkey_binding_lookup_ignorable_modifiers(GdkKeymap *keymap)
{
	egg_keymap_resolve_virtual_modifiers (keymap, EGG_VIRTUAL_LOCK_MASK,
			&caps_lock_mask);
	egg_keymap_resolve_virtual_modifiers (keymap, EGG_VIRTUAL_NUM_LOCK_MASK,
			&num_lock_mask);
	egg_keymap_resolve_virtual_modifiers (keymap,
			EGG_VIRTUAL_SCROLL_LOCK_MASK, &scroll_lock_mask);
}

static GdkFilterReturn gkey_binding_filter_func(GdkXEvent *gdk_xevent,
		GdkEvent *event, gpointer data)
{
	GdkFilterReturn return_val = GDK_FILTER_CONTINUE;
	XEvent *xevent = (XEvent *) gdk_xevent;
	guint event_mods;
	GSList *iter;

	switch (xevent->type) {
	case KeyPress:
		/* 
		 * Set the last event time for use when showing
		 * windows to avoid anti-focus-stealing code.
		 */
		processing_event = TRUE;
		last_event_time = xevent->xkey.time;

		event_mods = xevent->xkey.state & ~(num_lock_mask  |
				caps_lock_mask |
				scroll_lock_mask);

		for (iter = bindings; iter != NULL; iter = iter->next) {
			GkeyBinding *binding = (GkeyBinding *)iter->data;

			if (binding->keycode == xevent->xkey.keycode &&
					binding->modifiers == event_mods) {
				g_signal_emit(binding, GKEY_BINDING_GET_CLASS(
						binding)->pressed_signal_id,
						0, NULL);
			}
		}

		processing_event = FALSE;
		break;
	case KeyRelease:
		break;
	}

	return return_val;
}

static void gkey_binding_regrab(GkeyBinding *gb)
{
	gchar *s = g_strdup(gb->keystring);
	gkey_binding_grab(gb, s);
	g_free(s);
}

static void gkey_binding_keymap_changed(GdkKeymap *map)
{
        GdkKeymap *keymap = gdk_keymap_get_default();
        GSList *iter;

        for (iter = bindings; iter != NULL; iter = iter->next) {
                GkeyBinding *binding = (GkeyBinding *)iter->data;
                gkey_binding_ungrab(binding);
        }

	gkey_binding_lookup_ignorable_modifiers(keymap);
                                
        for (iter = bindings; iter != NULL; iter = iter->next) {
                GkeyBinding *binding = (GkeyBinding *) iter->data;
                gkey_binding_regrab(binding);
        }
}

/*
 * helper routine
 */
guint32 gkey_binding_get_current_event_time(void)
{
        if (processing_event)
                return last_event_time;
        else
                return GDK_CURRENT_TIME;
}

