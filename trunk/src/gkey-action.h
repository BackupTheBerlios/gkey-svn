#ifndef __GKEY_ACTION_H__
#define __GKEY_ACTION_H__

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include <glib-object.h>
#include <glib/gtypes.h>
#include <regex.h>

G_BEGIN_DECLS

#define GKEY_TYPE_ACTION            	(gkey_action_get_type())
#define GKEY_ACTION(o)             	(G_TYPE_CHECK_INSTANCE_CAST((o), \
					GKEY_TYPE_ACTION, GkeyAction))
#define GKEY_ACTION_CLASS(k)       	(G_TYPE_CHECK_CLASS_CAST((k), \
					GKEY_TYPE_ACTION, \
					GkeyActionClass))
#define GKEY_IS_ACTION(obj)	      	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
					GKEY_TYPE_ACTION))
#define GKEY_IS_ACTION_CLASS(klass) 	(G_TYPE_CHECK_CLASS_TYPE ((klass), \
					GKEY_TYPE_ACTION))
#define GKEY_ACTION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), \
					GKEY_TYPE_ACTION, \
					GkeyActionClass))

typedef struct _GkeyAction GkeyAction;
typedef struct _GkeyActionClass GkeyActionClass;

struct _GkeyAction {
	GObject parent;

	regex_t *res_name;
	regex_t *res_class;
	regex_t *window_name;
	regex_t *application_name;
};

struct _GkeyActionClass {
	GObjectClass parent;

	void (*run)(GkeyAction *);
};

GType gkey_action_get_type(void);
GkeyAction *gkey_action_new();

/*
 * Method definitions.
 */

void gkey_action_run(GkeyAction *);
WnckWindow *gkey_action_find_window(GkeyAction *self);

void regexpr_free(regex_t *reg_expr);
void regexpr_init(regex_t **reg_expr, const gchar *str);
gboolean regexpr_match(regex_t *reg_expr, const gchar *str);

G_END_DECLS

#endif /* __GKEY_ACTION_H__ */
