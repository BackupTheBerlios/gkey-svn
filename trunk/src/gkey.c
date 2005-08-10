/*
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <glib/glist.h>
#include <stdlib.h>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include "gkey-binding.h"
#include "gkey-activator.h"
#include "gkey-mousemover.h"

void read_conf(void);

int main(int argc, char **argv)
{
	/* Initialise GTK+ */
	gtk_init(&argc, &argv);

	gkey_binding_system_init();

	read_conf();

	gtk_main();
	return 0;
}

