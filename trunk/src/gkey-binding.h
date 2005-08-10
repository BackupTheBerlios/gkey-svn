#ifndef __KEY_BINDING_H__
#define __KEY_BINDING_H__

#include <glib-object.h>
#include <glib/gtypes.h>

G_BEGIN_DECLS

#define GKEY_TYPE_BINDING         (gkey_binding_get_type())
#define GKEY_BINDING(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), GKEY_TYPE_BINDING, GkeyBinding))
#define GKEY_BINDING_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
			GKEY_TYPE_BINDING, GkeyBindingClass))
#define GKEY_IS_BINDING(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GKEY_TYPE_BINDING))
#define GKEY_IS_BINDING_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GKEY_TYPE_BINDING))
#define GKEY_BINDING_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GKEY_TYPE_BINDING, GkeyBindingClass))



typedef struct _GkeyBinding GkeyBinding;
typedef struct _GkeyBindingClass GkeyBindingClass;

struct _GkeyBinding {
	GObject parent;

	/* private */
	char *keystring;
	guint keycode;
	guint modifiers;
	gboolean grabbed;
};

struct _GkeyBindingClass {
	GObjectClass parent;

	/* signals */
	guint pressed_signal_id;
};

GType gkey_binding_get_type(void);

/*
 * Method definitions.
 */

GkeyBinding *gkey_binding_new();
void gkey_binding_ungrab(GkeyBinding *gb);
gint gkey_binding_grab(GkeyBinding *gb, char *);

/*
 * Static stuff
 */
guint32 gkey_binding_get_current_event_time(void);
void gkey_binding_system_init(void);

G_END_DECLS

#endif /* __KEY_BINDING_H__ */
