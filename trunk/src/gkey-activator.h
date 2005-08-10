#ifndef __GKEY_ACTIVATOR_H__
#define __GKEY_ACTIVATOR_H__

#include <glib-object.h>
#include <glib/gtypes.h>
#include "gkey-action.h"

G_BEGIN_DECLS

#define GKEY_TYPE_ACTIVATOR            	(gkey_activator_get_type())
#define GKEY_ACTIVATOR(o)             	(G_TYPE_CHECK_INSTANCE_CAST((o), \
					GKEY_TYPE_ACTIVATOR, GkeyActivator))
#define GKEY_ACTIVATOR_CLASS(k)       	(G_TYPE_CHECK_CLASS_CAST((k), \
					GKEY_TYPE_ACTIVATOR, \
					GkeyActivatorClass))
#define GKEY_IS_ACTIVATOR(obj)	      	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
					GKEY_TYPE_ACTIVATOR))
#define GKEY_IS_ACTIVATOR_CLASS(klass) 	(G_TYPE_CHECK_CLASS_TYPE ((klass), \
					GKEY_TYPE_ACTIVATOR))
#define GKEY_ACTIVATOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), \
					GKEY_TYPE_ACTIVATOR, \
					GkeyActivatorClass))

typedef struct _GkeyActivator GkeyActivator;
typedef struct _GkeyActivatorClass GkeyActivatorClass;

struct _GkeyActivator {
	GkeyAction parent;
};

struct _GkeyActivatorClass {
	GkeyActionClass parent;
};

GType gkey_activator_get_type(void);
GkeyActivator *gkey_activator_new();

G_END_DECLS

#endif /* __GKEY_ACTIVATOR_H__ */
