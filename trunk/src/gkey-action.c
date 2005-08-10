#include <string.h>
#include <gdk/gdk.h>
#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>

#include "gkey-action.h"

GType gkey_action_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (GkeyActionClass),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			sizeof (GkeyAction),
			0,
			NULL,
			NULL
		};
		type = g_type_register_static(G_TYPE_OBJECT,
				"GkeyActionType", &info, 0);
	}
	return type;
}

void gkey_action_run(GkeyAction *self)
{
	GKEY_ACTION_GET_CLASS(self)->run(self);
}

/*
 * protected methods
 */

WnckWindow *gkey_action_find_window(GkeyAction *self)
{
	/* find window to activate */
	GdkDisplay *display;
	int i, num_screens;
	GList *iter;
	gboolean matched = FALSE;
	WnckWindow *ww = NULL;

	display = gdk_display_get_default();
	g_assert(display != NULL);
	num_screens = gdk_display_get_n_screens(display);
	for (i = 0; i < num_screens; i++) {
		GList *winlist;
		WnckScreen *screen;

		screen = wnck_screen_get(i);
		winlist = wnck_screen_get_windows(screen);
		for (iter = winlist; iter; iter = iter->next) {
			ww = (WnckWindow *)iter->data;
			XClassHint xch;
			gboolean res_name = TRUE, res_class = TRUE;
			gboolean win_name = TRUE, app_name = TRUE;

			xch.res_name = NULL;
			xch.res_class = NULL;

			if (self->res_name || self->res_class)
				XGetClassHint(GDK_DISPLAY(),
						wnck_window_get_xid(ww),
						&xch);

			if (self->res_name &&
				!regexpr_match(self->res_name, xch.res_name))
				res_name = FALSE;

			if (self->res_class &&
				!regexpr_match(self->res_class, xch.res_class))
				res_class = FALSE;

			if (self->res_name || self->res_class) {
				XFree(xch.res_name);
				XFree(xch.res_class);
			}

			if (self->window_name &&
				!regexpr_match(self->window_name,
						wnck_window_get_name(ww)))
				win_name = FALSE;

			if (self->application_name &&
				!regexpr_match(self->application_name,
						wnck_application_get_name(ww)))
				app_name = FALSE;

			if (res_class && res_name && app_name && win_name) {
				matched = TRUE;
				break;
			}
		}
	}

	if (!matched)
		return NULL;

	return ww;
}

/*
 * Regexp helper functions, ripped from devilspie. Oh well, I'm lazy and the
 * resulting api is trivial to use, why bother.
 */

void regexpr_free(regex_t *reg_expr)
{
	if (reg_expr) {
		regfree(reg_expr);
		g_free(reg_expr);
	}
}

void regexpr_init(regex_t **reg_expr, const gchar *str)
{
	int res;

	if (!str) {
		regexpr_free(*reg_expr);
		*reg_expr = NULL;
	} else {
		if (!*reg_expr) {
			*reg_expr = g_new(regex_t, 1);
		} else {
			regfree(*reg_expr);
		}
		res = regcomp(*reg_expr, str, REG_EXTENDED|REG_NOSUB);
		if (res != 0) {
			char buffer[255];
			regerror(res, *reg_expr, buffer, 255);
			g_warning("Invalid regular expression '%s': %s", str,
					buffer);
			*reg_expr = NULL;
		}
	}
}

gboolean regexpr_match(regex_t *reg_expr, const gchar *str)
{
	return (regexec(reg_expr, str, 0, NULL, 0) == 0) ? TRUE : FALSE;
}
