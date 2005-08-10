#include <gdk/gdk.h>
#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include "gkey-mousemover.h"
#include <string.h>

static void gkey_mousemover_class_init(gpointer g_class, gpointer g_class_data);
static void gkey_mousemover_real_run(GkeyAction *self);

GType gkey_mousemover_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (GkeyMousemoverClass),
			NULL,
			NULL,
			gkey_mousemover_class_init,
			NULL,
			NULL,
			sizeof (GkeyMousemover),
			0,
			NULL,
			NULL
		};
		type = g_type_register_static(GKEY_TYPE_ACTION,
				"GkeyMousemoverType", &info, 0);
	}
	return type;
}

static void gkey_mousemover_class_init(gpointer g_class, gpointer g_class_data)
{
	GkeyActionClass *klass = GKEY_ACTION_CLASS(g_class);
	klass->run = gkey_mousemover_real_run;
}

static void gkey_mousemover_real_run(GkeyAction *self)
{
	GkeyMousemover *gm = GKEY_MOUSEMOVER(self);
	WnckWindow *ww;
	
	ww = gkey_action_find_window(self);

	if (!ww)
		return;

	XWarpPointer(GDK_DISPLAY(), None, wnck_window_get_xid(ww),
				0, 0, 0, 0, gm->mouse_x, gm->mouse_y);
}

/*
 * public instance methods
 */

GkeyMousemover *gkey_mousemover_new()
{
	GkeyMousemover *gm =
		GKEY_MOUSEMOVER(g_object_new(GKEY_TYPE_MOUSEMOVER, NULL));
	GkeyAction *ga = GKEY_ACTION(gm);

	ga->res_name = NULL;
	ga->res_class = NULL;
	ga->window_name = NULL;
	ga->application_name = NULL;
	gm->mouse_x = 0;
	gm->mouse_y = 0;
	return gm;
}
