#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h> 

#ifndef GRAPHS_H
#define GRAPHS_H

void f_array(const gfloat *x_values, gfloat *y_values, gsize array_size);
static gboolean on_draw_graph(GtkWidget *widget, cairo_t *cr, gpointer user_data);

#endif