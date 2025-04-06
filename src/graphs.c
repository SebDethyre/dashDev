#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "../include/graphs.h" 

// Calculate y values from x values
void f_array(const gfloat *x_values, gfloat *y_values, gsize array_size) {
    for (gsize i = 0; i < array_size; i++) {
        y_values[i] = x_values[i]; // Any function here
    }
}

static gboolean on_draw_graph(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    gdouble zoom_x = 100;
    gdouble zoom_y = 100;
    GdkRectangle da;
    gdouble dx = 5.0, dy = 5.0;
    gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    GdkWindow *window = gtk_widget_get_window(widget);

    gdk_window_get_geometry(window, &da.x, &da.y, &da.width, &da.height);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_paint(cr);

    cairo_translate(cr, 0, da.height); // Move origin to bottom-left corner
    cairo_scale(cr, zoom_x, -zoom_y);  // Invert Y-axis

    cairo_device_to_user_distance(cr, &dx, &dy);
    cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width(cr, dx);

    // Draw x-axis from -1 to clip_x2
    cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
    cairo_move_to(cr, -1.0, 0.0);
    cairo_line_to(cr, clip_x2, 0.0);
    cairo_stroke(cr);

    // Draw y-axis from -1 to clip_y2
    cairo_move_to(cr, 0.0, -1.0);
    cairo_line_to(cr, 0.0, clip_y2);
    cairo_stroke(cr);

    gsize num_points = 3; // Number of points to draw
    gfloat x_values[num_points]; // Abscisses are indices
    gfloat y_values[] = {2.0, 1.0, 3.0}; // Sample ordinate values

    for (gsize i = 0; i < num_points; i++) {
        x_values[i] = i;
    }

    // Move to the first point
    cairo_move_to(cr, x_values[-1], y_values[0]);

    // Draw lines connecting the points
    for (gsize i = 1; i < num_points; i++) {
        cairo_line_to(cr, x_values[i], y_values[i]);
    }

    cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
    cairo_stroke(cr);

    return FALSE;
}

/******* Usage *******/ 

// GtkWidget *window;
// GtkWidget *da;
// gtk_init (&argc, &argv);

// window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
// gtk_window_set_default_size (GTK_WINDOW (window), 640, 480);
// gtk_window_set_title (GTK_WINDOW (window), "Graph drawing");
// g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

// da = gtk_drawing_area_new();
// gtk_container_add(GTK_CONTAINER(window), da);

// g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_draw), NULL);      