#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h> 

#ifndef LAUNCHING_H
#define LAUNCHING_H

void launch_file(GtkTreeView *treeview, gchar *filename, const gchar *appli);
void launch_test(GtkTreeView *treeview, const gchar *appli);

#endif