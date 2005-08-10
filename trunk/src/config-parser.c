/* shamelessly ripped from devilspie */

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <stdio.h>
#include <string.h>
#include "gkey-action.h"
#include "gkey-binding.h"
#include "gkey-activator.h"
#include "gkey-mousemover.h"

typedef struct _Node {
	gchar *name;
	struct _Node *parent;
	GList *sub_nodes;
	gchar *att_name;
	gchar *att_value;
} Node;

typedef struct _Parser {
	Node *root_node;
	GList *node_stack;
} Parser;

static void conf_fatal(char *s)
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}

Node *node_new(const gchar *name, Node *parent, gchar *aname, gchar *aval)
{
	Node *n = g_new0(Node, 1);
	g_assert(name != NULL);
	n->name = g_strdup(name);
	n->parent = parent;
#define MAYDUP(s) ((s == NULL)?NULL:g_strdup(s))
	n->att_name = MAYDUP(aname);
	n->att_value = MAYDUP(aval);
#undef MAYDUP
	return n;
}

static void start_element_handler(GMarkupParseContext *context,
                       const gchar         *element_name,
                       const gchar        **attribute_names,
                       const gchar        **attribute_values,
                       gpointer             user_data,
                       GError             **error)
{
	(void)context;
	(void)error;

	Parser *parser = (Parser *)user_data;
	const gchar **name, **val;
	Node *parent;
	
	if (parser->root_node == NULL) {
		if (strcmp(element_name, "gkey") != 0)
			conf_fatal("root is not gkey");
		parser->root_node = node_new(element_name, NULL, NULL, NULL);
		parser->node_stack = g_list_prepend(parser->node_stack,
				parser->root_node);
		return;
	}

	parent = (Node*)(g_list_first(parser->node_stack)->data);
	gchar *n = NULL, *v = NULL;

	for (name = attribute_names, val = attribute_values; *name;
			name++, val++) {
		if (strcmp(*name, "name") == 0)
			n = g_strdup(*val);
		else if (strcmp(*name, "value") == 0)
			v = g_strdup(*val);
		else
			conf_fatal("unknown attribute");
	}

	/* quick'n'dirty xml checking */
	if (strcasecmp(element_name, "key") == 0) {
		if (strcasecmp(parent->name, "gkey") != 0)
			conf_fatal("key element must be in gkey\n");
		if (!v)
			conf_fatal("key element needs a value attribute\n");
	} else if (strcasecmp(element_name, "action") == 0) {
		if (strcasecmp(parent->name, "key") != 0)
			conf_fatal("action element must be in key\n");
		if (!n)
			conf_fatal("action element needs a name "
						"attribute\n");
		if (strcasecmp(n, "activate") != 0 && 
				strcasecmp(n, "movemouse") != 0)
			conf_fatal("action's name needs to be eigher "
					"Activate or MoveMouse\n");
	} else if (strcasecmp(element_name, "window") == 0) {
		if (strcasecmp(parent->name, "action") != 0)
			conf_fatal("window element must be in action\n");
	} else if (strcasecmp(element_name, "property") == 0) {
		if (strcasecmp(parent->name, "action") != 0 && 
			strcasecmp(parent->name, "window") != 0)
			conf_fatal("misplace property element\n");
		if (!n || !v)
			conf_fatal("property needs name and value\n");
	}

	Node *newnode = node_new(element_name, parent, n, v);
	parent->sub_nodes = g_list_append(parent->sub_nodes, newnode);
	parser->node_stack = g_list_prepend(parser->node_stack, newnode);
}

static void end_element_handler(GMarkupParseContext  *context,
		const gchar          *element_name,
		gpointer              user_data,
		GError              **error)
{
	(void)context;
	(void)element_name;
	(void)error;

	Parser *parser = (Parser *)user_data;
	parser->node_stack = g_list_remove(parser->node_stack,
			g_list_first(parser->node_stack)->data);
}

static void text_handler(GMarkupParseContext  *context,
                          const gchar          *text,
                          gsize                 text_len,
                          gpointer              user_data,
                          GError              **error)
{
	(void)context;
	(void)user_data;
	(void)error;
	guint i;
	for (i = 0; i < text_len; i++) {
		if (!g_unichar_isspace(text[i])) {
			conf_fatal("No text allowed in gkey config\n");
		}
	}
}

static GMarkupParser gkey_config_parser = {
	start_element_handler,
	end_element_handler,
	text_handler,
	NULL,
	NULL
};

void gkey_run_cb(gpointer self, gpointer data)
{
	(void)self;
	gkey_action_run(GKEY_ACTION(data));
}

void run_tree(Node *n)
{
	static GkeyBinding *gbinding = NULL;
	static GkeyAction *gaction = NULL;
	static GkeyMousemover *gmousemover = NULL;
	gboolean err = 0;

	if (strcmp(n->name, "key") == 0) {
		gbinding = gkey_binding_new();
		err = !gkey_binding_grab(gbinding, n->att_value);
		if (err)
			g_warning("could not grab %s\n", n->att_value);
	} else if (strcasecmp(n->name, "action") == 0) {
		if (strcasecmp(n->att_name, "activate") == 0) {
			GkeyActivator *gactivator;
			gactivator = gkey_activator_new();
			gaction = GKEY_ACTION(gactivator);
			g_signal_connect(gbinding, "pressed",
					(GCallback)gkey_run_cb, gaction);
		} else if (strcasecmp(n->att_name, "movemouse") == 0) {
			gmousemover = gkey_mousemover_new();
			gaction = GKEY_ACTION(gmousemover);
			g_signal_connect(gbinding, "pressed",
					(GCallback)gkey_run_cb, gaction);
		} else {
			printf("%s\n", n->att_name);
			conf_fatal("Unknown action name\n");
		}
	} else if (strcasecmp(n->name, "window") == 0) {
		/* ignored */
	} else if (strcasecmp(n->name, "property") == 0) {
		if (strcasecmp(n->att_name, "res_name") == 0)
			regexpr_init(&gaction->res_name, n->att_value);
		if (strcasecmp(n->att_name, "res_class") == 0)
			regexpr_init(&gaction->res_class, n->att_value);
		if (strcasecmp(n->att_name, "window_name") == 0)
			regexpr_init(&gaction->window_name, n->att_value);
		if (strcasecmp(n->att_name, "application_name") == 0)
			regexpr_init(&gaction->application_name, n->att_value);
		if (strcasecmp(n->att_name, "x") == 0) {
			if (!gmousemover)
				conf_fatal("misused x attribute");
			gmousemover->mouse_x = atoi(n->att_value);
		}
		if (strcasecmp(n->att_name, "y") == 0) {
			if (!gmousemover)
				conf_fatal("misused x attribute");
			gmousemover->mouse_y = atoi(n->att_value);
		}
	} 

	if (!err) {
		GList *cur;
		for (cur = n->sub_nodes; cur; cur = cur->next)
			run_tree((Node *)(cur->data));
	}
}

void load_configuration(const char *filename)
{
	char *text;
	unsigned int length;
	GError *error;
	GMarkupParseContext *context;
	Parser *p;

	error = NULL;
	if (!g_file_get_contents(filename, &text, &length, &error)) {
		g_warning("Could not load theme: %s", error->message);
		exit(1);
	}
	/* Sanity check */
	g_assert(text);

	p = g_new0(Parser, 1);
	context = g_markup_parse_context_new(&gkey_config_parser, 0, p, NULL);
	error = NULL;
	if (g_markup_parse_context_parse(context, text, length, &error))
		if (g_markup_parse_context_end_parse(context, &error))
			g_markup_parse_context_free(context);

	g_free (text);

	if (error) {
		g_warning("Could not parse configuration: %s", error->message);
		exit(1);
	}

	run_tree(p->root_node);
}

void read_conf(void)
{
	char *filename;
	filename = g_strdup_printf("%s/%s", g_get_home_dir(), ".gkey.xml");
	load_configuration(filename);
	g_free(filename);
}

