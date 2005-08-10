#include <string.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include "gkey-activator.h"
#include "gkey-binding.h"

static void gkey_activator_class_init(gpointer g_class, gpointer g_class_data);
static void gkey_activator_real_run(GkeyAction *self);

GType gkey_activator_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (GkeyActivatorClass),
			NULL,
			NULL,
			gkey_activator_class_init,
			NULL,
			NULL,
			sizeof (GkeyActivator),
			0,
			NULL,
			NULL
		};
		type = g_type_register_static(GKEY_TYPE_ACTION,
				"GkeyActivatorType", &info, 0);
	}
	return type;
}

static void gkey_activator_class_init(gpointer g_class, gpointer g_class_data)
{
	GkeyActionClass *klass = GKEY_ACTION_CLASS(g_class);
	klass->run = gkey_activator_real_run;
}

static void gkey_activator_real_run(GkeyAction *self)
{
	/* find window to activate */
	WnckWindow *ww;
	ww = gkey_action_find_window(self);

	if (!ww)
		return;

	/* change desktop */
	wnck_workspace_activate(wnck_window_get_workspace(ww),
			gkey_binding_get_current_event_time());
	/* activate window */
	wnck_window_activate(ww, gkey_binding_get_current_event_time());
}

/*
 * public instance methods
 */

GkeyActivator *gkey_activator_new()
{
	GkeyActivator *ga =
		GKEY_ACTIVATOR(g_object_new(GKEY_TYPE_ACTIVATOR, NULL));
	GkeyAction *gact = GKEY_ACTION(ga);

	gact->res_name = NULL;
	gact->res_class = NULL;
	gact->window_name = NULL;
	gact->application_name = NULL;
	return ga;
}
