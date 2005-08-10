#ifndef __GKEY_MOUSEMOVER_H__
#define __GKEY_MOUSEMOVER_H__

#include <glib-object.h>
#include <glib/gtypes.h>
#include "gkey-action.h"

G_BEGIN_DECLS

#define GKEY_TYPE_MOUSEMOVER           	(gkey_mousemover_get_type())
#define GKEY_MOUSEMOVER(o)             	(G_TYPE_CHECK_INSTANCE_CAST((o), \
					GKEY_TYPE_MOUSEMOVER, GkeyMousemover))
#define GKEY_MOUSEMOVER_CLASS(k)       	(G_TYPE_CHECK_CLASS_CAST((k), \
					GKEY_TYPE_MOUSEMOVER, \
					GkeyMousemoverClass))
#define GKEY_IS_MOUSEMOVER(obj)	      	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
					GKEY_TYPE_MOUSEMOVER))
#define GKEY_IS_MOUSEMOVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
					GKEY_TYPE_MOUSEMOVER))
#define GKEY_MOUSEMOVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
					GKEY_TYPE_MOUSEMOVER, \
					GkeyMousemoverClass))

typedef struct _GkeyMousemover GkeyMousemover;
typedef struct _GkeyMousemoverClass GkeyMousemoverClass;

struct _GkeyMousemover {
	GkeyAction parent;

	gint mouse_x;
	gint mouse_y;
};

struct _GkeyMousemoverClass {
	GkeyActionClass parent;
};

GType gkey_mousemover_get_type(void);
GkeyMousemover *gkey_mousemover_new();

/*
 * Method definitions.
 */

G_END_DECLS

#endif /* __GKEY_MOUSEMOVER_H__ */
