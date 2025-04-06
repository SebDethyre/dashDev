#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h> 
#include <sys/wait.h> 
#include <sys/stat.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <float.h>
#include <time.h>
#include <ctype.h>
#include <dirent.h>

#include "include/fileman.h"
#include "include/launching.h"
// #include "include/graphs.h"
// mIeLvWHXVJjwfqp2
#define LOCK_FILE "/tmp/dash_dev.lock"

int        current_index                =-1;
int        selected_value               = 1;
gint       window_width;
gint       number_of_devs, number_of_applis;
gint       basic_height; 
gint       intern_minal_height; 
gint       y_values[1000] = {};

guint      neon_blink_timeout_id_less   = 0;
guint      neon_blink_timeout_id_return = 0;
guint      neon_blink_timeout_id_ctrl_D = 0;
guint      neon_blink_timeout_id_alt_gr = 0;
guint      neon_blink_timeout_id_space  = 0;
guint      neon_blink_timeout_id_n      = 0;
 
gulong     key_press_handler_id;
gulong     key_press_handler_id_B;
gulong     key_press_handler_id_launch;
gdouble    percent_by_time;
gdouble    percent_by_lines;

GtkWidget *check_vs, *check_ti, *check_job, *check_app, *check_init, *commit_window, *checkout_window,*menu = NULL, *combobox, *combobox_appli, *counter_label;
GtkWidget *label1, *label2, *label3, *label4, *label5, *label6, *label7, *label8, *label9;
GtkWidget *output_text_view = NULL;
GtkWidget *slider_speed;
GtkWidget *slider_skip;
GtkWindow *window;
// GtkWidget *window;
GtkWidget *popup_info                   = NULL;
GtkWidget *popup_builds                 = NULL;
GtkWidget *popup_graph                  = NULL;
GtkWidget *scrolled_window;
GtkWidget *drawing_area;
GtkWidget *vbox;
GtkButton *terminal_button;

gboolean   neon_blink_state_ctrl        = FALSE;
gboolean   terminal_open                = FALSE;
gboolean   show_last_commit             = TRUE;

gchar     *intern_minal_longest_string;
const gchar *current_appli = NULL;

gchar     *branch_selected_l            = NULL; 
const char *pidFilename = "/home/sdethyre/sandbox/dash_dev_boisson/pid.txt";
GtkWidget *button_tests;
GtkWidget *button_test_cases;
GtkTreeView *treeview;
typedef struct {
    gint index;
    GtkTreeView *treeview;
} MenuItemData;

typedef struct {
    GtkWidget *popup_box;
    gchar *script_name;
    gchar *branch_name;
} CreatePopupParams;

//📺 📊 📇
const gchar* array_du_select_L[] = { "📈     Jenkins", "📊     Job", "📉     Fails", "📰     Output", "🔬     Report", "📋     Logs RF", "📺     Logs APV", "📑     Ticket"};
const int    num_options_L = sizeof(array_du_select_L) / sizeof(array_du_select_L[0]);

const gchar* array_du_select_R[] = { "🌀     Meld", "📜     Résumé", "📝     Commit", "🗑     Delete", "📟     Statut", "👥     Diff", "🌌     Animation", "🔗     Checkout", "📩     Up"};
const int    num_options_R = sizeof(array_du_select_R) / sizeof(array_du_select_R[0]);

const gchar* array_du_select_B[] = { "🔄     Appli Init", "🔀     Merge","👣    Meld branches",  "📜     Poedit", "🆕     Nouveau" , "🗄     AdMiner" };
const int    num_options_B = sizeof(array_du_select_B) / sizeof(array_du_select_B[0]);

const gchar* array_du_select_V[] = { "📈     Jenkins", "📊     Job", "🌀     Meld", "📑     Ticket", "📋     Logs", "🆕     Nouveau", "━━━━━━━━━━", "📩     Up", "📝     Commit", "🔀     Merge", "📜     Résumé", "📟     Statut", "👥     Diff" , "🗑     Delete"};
const int    num_options_V = sizeof(array_du_select_V) / sizeof(array_du_select_V[0]);

bool is_instance_running() {
    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("Could not open lock file");
        exit(EXIT_FAILURE);
    }
    int lock_result = flock(lock_fd, LOCK_EX | LOCK_NB);
    if (lock_result == -1) {
        close(lock_fd);
        return true;
    }
    return false;
}

void replace_substring_in_file(const char *filename, const char *largerSubstring, const char *substringToReplace, const char *newSubstring) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *fileContent = malloc(fileSize + 1);
    if (fileContent == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(fileContent, 1, fileSize, file);
    fclose(file);

    // Ajouter un caractère de fin de chaîne
    fileContent[fileSize] = '\0';

    // Vérifier que la petite sous-chaîne n'est présente qu'à l'intérieur de la plus grande sous-chaîne
    if (strstr(largerSubstring, substringToReplace) == NULL) {
        printf("La petite sous-chaîne n'est pas contenue dans la plus grande sous-chaîne.\n");
        free(fileContent);
        exit(EXIT_FAILURE);
    }

    // Trouver la position de la plus grande sous-chaîne
    char *largerSubstringPosition = strstr(fileContent, largerSubstring);

    while (largerSubstringPosition != NULL) {
        // Trouver la position de la petite sous-chaîne à l'intérieur de la plus grande sous-chaîne
        char *substringPosition = strstr(largerSubstringPosition, substringToReplace);

        if (substringPosition != NULL) {
            // Remplacer la petite sous-chaîne
            memcpy(substringPosition, newSubstring, strlen(newSubstring));

            // Chercher la prochaine occurrence de la plus grande sous-chaîne dans la ligne
            largerSubstringPosition = strstr(substringPosition + strlen(newSubstring), largerSubstring);
        } else {
            // Si la petite sous-chaîne à l'intérieur de la plus grande sous-chaîne n'est pas trouvée, sortir de la boucle
            break;
        }
    }

    // Réécrire le contenu modifié dans le fichier
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier en écriture");
        exit(EXIT_FAILURE);
    }

    fwrite(fileContent, 1, fileSize, file);
    fclose(file);

    // Libérer la mémoire allouée
    free(fileContent);
}

struct tm adjustDate(int monthsToSubtract, int daysToSubtract) {
    time_t rawtime;
    struct tm timeinfo;

    // Obtenir la date actuelle
    time(&rawtime);
    timeinfo = *localtime(&rawtime);

    // Ajouter ou soustraire des mois
    timeinfo.tm_mon -= monthsToSubtract;

    // Ajouter ou soustraire des jours
    timeinfo.tm_mday -= daysToSubtract;

    // Utiliser mktime pour normaliser la date
    mktime(&timeinfo);

    return timeinfo;
}

void grow_intern_minal() {
    gtk_window_resize(GTK_WINDOW(window), window_width, basic_height + 300);
    gtk_widget_show(scrolled_window);
    gtk_widget_set_size_request(GTK_WIDGET(scrolled_window), -1, 300);
    gtk_button_set_label(terminal_button, "▲");
    terminal_open = TRUE;
}

void shrink_intern_minal() {
    gtk_window_resize(GTK_WINDOW(window), window_width , basic_height);
    gtk_widget_hide(scrolled_window);
    gtk_button_set_label(terminal_button, "▼");
    terminal_open = FALSE;
}

void save_current_dev(GtkTreeView *treeview) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        shrink_intern_minal();
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        // Convertion du chemin en un indice d'itération (index) dans le modèle
        int index = gtk_tree_path_get_indices(path)[0];
        // Libération de mémoire du GtkTreePath
        gtk_tree_path_free(path);
        // Mise à jour de la variable globale de l'index
        current_index = index;
        gchar *current_dev_appli_entry;

        if (strcmp(current_appli, "openads") == 0) {
            current_dev_appli_entry = "current_dev_ads:";
        }
        if (strcmp(current_appli, "openaria") == 0) {
            current_dev_appli_entry = "current_dev_aria:";
        }
        if (strcmp(current_appli, "opencimetiere") == 0) {
            current_dev_appli_entry = "current_dev_cimetierre:";
        }
        if (strcmp(current_appli, "opendebitdeboisson") == 0) {
            current_dev_appli_entry = "current_dev_debit:";
        }
        if (strcmp(current_appli, "openelec") == 0) {
            current_dev_appli_entry = "current_dev_elec:";
        }
        if (strcmp(current_appli, "openresultat") == 0) {
            current_dev_appli_entry = "current_dev_resultat:";
        }
        
        // gchar *index_temp = g_strdup_printf("current_dev:%d", current_index);
        gchar *index_temp = g_strdup_printf("%s%d", current_dev_appli_entry, current_index);
        // gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
        replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry, index_temp);
    } 
    else current_index = -1;
}

void generic_quit (GtkTreeView *treeview, const char *pidFilename) {
    save_current_dev(treeview);
    if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
    clear_file(pidFilename);
    gtk_main_quit();
}

void f_array(const gfloat *x_values, gfloat *y_values, gsize array_size) {
    // for (gsize i = 0; i < array_size; i++) { y_values[i] = x_values[i]; } // Any function here
    memcpy(y_values, x_values, array_size * sizeof(gfloat));
}

gboolean on_draw_graph(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    gsize num_points       = count_lines_in_file("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results")+1; 
    gdouble zoom_x         = 120;
    gdouble zoom_y         = 70;
    GdkRectangle da;
    gdouble dx             = 5.0, 
            dy             = 10.0;
    gdouble clip_x1        = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    GdkWindow *window      = gtk_widget_get_window(widget);
    gboolean too_big       = false;
    gboolean too_too_big   = false;
    gboolean jenkins_style = true;

    gdk_window_get_geometry(window, &da.x, &da.y, &da.width, &da.height);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_paint(cr);
    
    if (jenkins_style){
        // Sens "Jenkins" de lecture des fails
        gdouble center_x = clip_x2 / 2.0;
        gdouble center_y = (clip_y2 - clip_y1) / 2.0;
        cairo_translate(cr, center_x, center_y);
    } 
    else cairo_translate(cr, 0, da.height); // Move origin to bottom-left corner - Sens "naturel" de lecture des fails

    for (gsize i = 0; i  < num_points; i++) {
        if (y_values[i]  > 5) too_big = true;
        if (y_values[i]  > 10) too_too_big = true;
    }
    if (too_big) zoom_y     = 40;
    if (too_too_big) zoom_y = 20;

    if (num_points < 6) zoom_x = 150;
    if (num_points < 5) zoom_x = 200;
    if (num_points < 4) zoom_x = 300;
    
    if (jenkins_style) cairo_scale(cr, zoom_x,  zoom_y); // Sens "Jenkins" de lecture des fails
    else               cairo_scale(cr, zoom_x, -zoom_y);  // Invert Y-axis : Sens "naturel" de lecture des fails

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

    gfloat x_values[num_points]; // Abscisses are indices

    for (gsize i = 0; i < num_points; i++) {
        x_values[i] = i;
    }
    // Move to the first point
   cairo_move_to(cr, x_values[0], y_values[0]);

    // Draw lines connecting the points
    for (gsize i = 1; i < num_points; i++) {
        cairo_line_to(cr, x_values[i], y_values[i]);
    }
    if (jenkins_style) {
        // Avec remplissage 
        // Line back to the starting point to close the path
        cairo_line_to(cr, x_values[num_points - 1], 0);
        cairo_line_to(cr, x_values[0], 0);
        cairo_save(cr);
        // Dessiner tout ce qui est au-dessus de la courbe en rouge
        cairo_set_source_rgba(cr, 1, 0.1, 0.1, 0.6); // Rouge
        cairo_fill_preserve(cr); // Remplissage en rouge
        // Définir un masque pour tout ce qui est en dessous de la courbe
        cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OUT); // Utiliser l'opérateur DEST_OUT pour colorer seulement en dessous
        cairo_set_source_rgba(cr, 0.3, 1, 0.5, 0.6); // Vert
        cairo_restore(cr);
        // Dessiner un rectangle couvrant la zone en dessous de la courbe
        cairo_rectangle(cr, clip_x1, 0, clip_x2 - clip_x1, clip_y2);
        // Appliquer le masque
        cairo_fill(cr);
    }
    else {
        // Sans remplissage
        cairo_set_source_rgba(cr, 1, 0.2, 0.2, 0.6);
        cairo_stroke(cr);
    }
    return FALSE;
}
// Fonction de rappel appelée lors du redimensionnement de la fenêtre
void on_window_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data) {
    gint width = event->width;
    g_print("Largeur de la fenêtre : %d\n", width);
    
}

void toggle_intern_minal_height(GtkWidget *widget, GtkWidget *scrolled_window) {
    if (terminal_open) {
        gtk_widget_hide(scrolled_window);
        gtk_button_set_label(terminal_button, "▼");
        terminal_open = FALSE;
    }
    else {
        gtk_widget_show(scrolled_window);
        gtk_widget_set_size_request(scrolled_window, -1, 300);
        gtk_button_set_label(terminal_button, "▲");
        terminal_open = TRUE;
    }
}

void toggle_window_height_at_bottom(GtkWidget *widget, GtkWindow *window) {
    if (!terminal_open) {
        gtk_window_resize(window, window_width, basic_height);
        terminal_open = FALSE;
    }
    else {
        gtk_window_resize(window, window_width, basic_height + 300);
        terminal_open = TRUE;
    }
}

gchar* recuperer_elements_droite_string(const char* fichier, const char* chaine) {
    FILE* file = fopen(fichier, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", fichier);
        return NULL;
    }
    char ligne[256];
    gchar* mots = NULL;
    while (fgets(ligne, sizeof(ligne), file)) {
        if (strstr(ligne, chaine)) {
            // Trouver l'index de la chaîne "chaine" dans la ligne
            char* index = strstr(ligne, chaine);
            // Avancer l'index jusqu'au caractère suivant "chaine"
            index += strlen(chaine);
            // Copier la valeur de droite dans la variable "mots"
            mots = strdup(index);
            break;
        }
    }
    if (mots == NULL) return NULL;
    fclose(file);
    return mots;
}

gchar* escape_quotes(const gchar* input) {
    GString *output = g_string_new(NULL);
    const gchar *p = input;
    while (*p) {
        if (*p == '\'') g_string_append(output, "'\\''");
        else g_string_append_c(output, *p);
        p++;
    }
    return g_string_free(output, FALSE);
}

void element_neon_on(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "/home/sdethyre/sandbox/dash_dev_boisson/style.css", NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_class(context, "neon");
}

void apply_css_class_on_widget(GtkWidget *widget, gchar *class_name) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    if (!gtk_css_provider_load_from_path(provider, "/home/sdethyre/sandbox/dash_dev_boisson/style.css", &error)) {
        g_warning("Unable to load CSS file: %s", error->message);
        g_error_free(error);
    }

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_class(context, class_name);
}

void remove_css_class_from_widget(GtkWidget *widget, gchar *class_name) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_remove_class(context, class_name);
}

gboolean element_neon_off(GtkWidget *widget) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_remove_class(context, "neon");
    return GDK_EVENT_PROPAGATE;
}

void element_neon_custom_on(GtkWidget *widget, gchar *color) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "/home/sdethyre/sandbox/dash_dev_boisson/style.css", NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gchar *class_name = g_strdup_printf("neon-%s", color);
    gtk_style_context_add_class(context, class_name);
}

gboolean element_neon_custom_off(GtkWidget *widget, gchar *color) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gchar *class_name = g_strdup_printf("neon-%s", color);
    gtk_style_context_remove_class(context, class_name);
    return GDK_EVENT_PROPAGATE;
}

gboolean remove_neon_class_callback(gpointer user_data) {
    GtkWidget *widget = GTK_WIDGET(user_data);
    gtk_style_context_remove_class(gtk_widget_get_style_context(widget), "neon");
    return G_SOURCE_REMOVE; // Le rappel ne doit s'exécuter qu'une fois
}

void element_neon_on_blink_once(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "/home/sdethyre/sandbox/dash_dev_boisson/style.css", NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_class(gtk_widget_get_style_context(widget), "neon");

    g_timeout_add(500, (GSourceFunc)remove_neon_class_callback, widget);
}

void make_label_blink (guint neon_blink_timeout_id, GtkWidget *label){
    if (neon_blink_timeout_id == 0) {
        element_neon_on_blink_once(GTK_WIDGET(label));
    } 
    else {
        element_neon_off(GTK_WIDGET(label)); 
        g_source_remove(neon_blink_timeout_id);
        neon_blink_timeout_id = 0;
    }
}

void connect_signals(GtkWidget *widget) {
    g_signal_connect(widget, "enter-notify-event" , G_CALLBACK(element_neon_on ), NULL);
    g_signal_connect(widget, "leave-notify-event" , G_CALLBACK(element_neon_off), NULL);
    g_signal_connect(widget, "focus-in-event"     , G_CALLBACK(element_neon_on ), NULL);
    g_signal_connect(widget, "focus-out-event"    , G_CALLBACK(element_neon_off), NULL);
    g_signal_connect(widget, "motion-notify-event", G_CALLBACK(element_neon_on ), NULL);
    // si le widget est un conteneur, connecter les signaux à ses enfants
    if (GTK_IS_CONTAINER(widget)) {
        GtkContainer *container = GTK_CONTAINER(widget);
        GList *children = gtk_container_get_children(container);
        GList *child;
        for (child = children; child != NULL; child = g_list_next(child)) {
            GtkWidget *child_widget = GTK_WIDGET(child->data);
            connect_signals(child_widget);
        }
        g_list_free(children);
    }
}

gint max_line_length = 0;
static void find_and_update_longest_line_length(gint line_length) {
    if (line_length >= max_line_length) {
        max_line_length = line_length;
        // if (line_length > 80 && line_length < 150 ){
        if (line_length > 80){
            gtk_window_resize(GTK_WINDOW(window), window_width + 1 * line_length, basic_height + 300);
        }
        // if (line_length > 100 && line_length < 150 ){
        if (line_length > 100){
            gtk_window_resize(GTK_WINDOW(window), window_width + 2.5 * line_length, basic_height + 300);
        }
    }
    max_line_length = 0;
}
// feat: tri sur le libelle du select dossier_autorisation_type_detaille de la recherche des DI                                 = 92
// fix : &nbsp; sur la description de projet n'est pas pris en compte lors de la suppression des espaces non sécable. (#10240) = 123
gboolean on_stdout_output(GIOChannel *channel, GIOCondition condition, gpointer data) {
    if (!GTK_IS_TEXT_BUFFER(data)) {
        g_warning("on_stdout_output: Invalid data type. Expected GtkTextBuffer.");
        return FALSE;
    }
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    gchar *line = NULL;
    gsize length = 0;

    // Lire une ligne de la sortie standard
    if (g_io_channel_read_line(channel, &line, &length, NULL, NULL) == G_IO_STATUS_NORMAL) {
        // Mettre à jour le widget GtkTextView avec la ligne lue
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, line, length);
    // printf("Longest here: %ld",length);

        // gint line_length = g_utf8_strlen(line, -1);
        
        find_and_update_longest_line_length(length);
        g_free(line);
        // Faire défiler le GtkTextView vers le bas pour afficher la dernière sortie
        GtkAdjustment *v_adjust = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(GTK_TEXT_VIEW(output_text_view)));

        if (v_adjust) gtk_adjustment_set_value(v_adjust, gtk_adjustment_get_upper(v_adjust) - gtk_adjustment_get_page_size(v_adjust));
    }
    // Continuer à surveiller la sortie standard
    return TRUE;
}

void execute_bash_intern(gpointer data, gchar *command) {
    GtkWidget *output_text_view = GTK_WIDGET(data);
    // Get the GtkTextBuffer associated with the GtkTextView
    GtkTextBuffer *output_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_text_view));
    // Clear the GtkTextView
    gtk_text_buffer_set_text(output_text_buffer, "", -1);
    gchar *final_command = g_strdup_printf("bash -c \"%s\"", command);
    // Set up the GIOChannels to monitor the standard output and error
    GPid pid;
    GIOChannel *stdout_channel = NULL;
    GIOChannel *stderr_channel = NULL;
    GError *error = NULL;
    int stdout_fd, stderr_fd;

    gboolean success = g_spawn_async_with_pipes(
        NULL,               // working directory
        (gchar *[]){"/bin/bash", "-c", final_command, NULL},
        NULL,               // environment
        G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
        NULL,               // child setup function
        NULL,               // user data
        &pid,               // child PID
        NULL,               // standard input
        &stdout_fd,         // standard output (file descriptor)
        &stderr_fd,         // standard error (file descriptor)
        &error              // error
    );
    g_free(final_command);
    if (success) {
        // Convert file descriptors to GIOChannels
        stdout_channel = g_io_channel_unix_new(stdout_fd);
        stderr_channel = g_io_channel_unix_new(stderr_fd);
        // Connect the on_stdout_output function to monitor the standard output
        g_io_add_watch(stdout_channel, G_IO_IN, on_stdout_output, output_text_buffer);
        g_io_add_watch(stderr_channel, G_IO_IN, on_stdout_output, output_text_buffer); // Add watch for stderr too
        g_io_channel_unref(stdout_channel);
        g_io_channel_unref(stderr_channel);
    } 
    else g_error_free(error);
}

void launch_terminal(GtkTreeView *treeview, gchar *command, gchar *how_to_close, gchar *title) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if(strcmp(how_to_close, "merge") == 0) {
        GError *error = NULL;
        gchar *command_line[] = { "/bin/bash", "-c", "/home/sdethyre/sandbox/merge_boisson/./merge_boisson", NULL };
        if (!g_spawn_async(NULL, command_line, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            g_print("Erreur lors de l'exécution du script : %s\n", error->message);
            g_error_free(error);
        }
        return;
    } 
    if(strcmp(how_to_close, "meld") == 0) {
        GError *error = NULL;
        gchar *command_line[] = { "/bin/bash", "-c", "/home/sdethyre/sandbox/meld/./meld", NULL };
        if (!g_spawn_async(NULL, command_line, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            g_print("Erreur lors de l'exécution du script : %s\n", error->message);
            g_error_free(error);
        }
        return;
    } 
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *branch_selected;
        gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);

        if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
            gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s/%s", current_appli, branch_selected);

            if (chdir(directory) != 0) {
                g_print("Impossible de changer de répertoire.\n");
                g_free(directory);
                return;
            }
            gchar *final_command;
            if(strcmp(how_to_close, "intern_minal") == 0) {
                grow_intern_minal();
                execute_bash_intern(output_text_view, command); 
                printf("Longest : %d",max_line_length);
                return;
            } 
            if(strcmp(how_to_close, "last_commit") == 0) {
                grow_intern_minal();
                gchar *appli_name;
                if (strcmp(current_appli, "openads") == 0) {
                    appli_name = "openfoncier";
                }
                if (strcmp(current_appli, "openaria") == 0) {
                    appli_name = "openaria";
                }
                if (strcmp(current_appli, "opencimetiere") == 0) {
                    appli_name = "opencimetiere";
                }
                if (strcmp(current_appli, "opendebitdeboisson") == 0) {
                    appli_name = "openboisson";
                }
                if (strcmp(current_appli, "openelec") == 0) {
                    appli_name = "openelec";
                }
                if (strcmp(current_appli, "openresultat") == 0) {
                    appli_name = "openresultat";
                }
                gchar *command_tmp = g_strdup_printf("svn log -r HEAD:1 --limit 1 svn+ssh://sdethyre@scm.adullact.net/svn/%s/branches/%s", appli_name, branch_selected);
                execute_bash_intern(output_text_view, command_tmp); 
                return;
            } 
            // gchar *terminal_title = "Apache";
            if     (strcmp(how_to_close, "return_key") == 0) final_command = g_strdup_printf("gnome-terminal -- bash -c '%s; read -s -r -p \"Appuyez sur Entrée pour fermer le terminal...\"'"   , command);
            else if     (strcmp(how_to_close, "return_key_title") == 0) final_command = g_strdup_printf("gnome-terminal -- bash -c 'printf \"\\033]0;%s\\007\"; %s; read -s -r -p \"Appuyez sur Entrée pour fermer le terminal...\"'", title, command);
            else if(strcmp(how_to_close,    "any_key") == 0) final_command = g_strdup_printf("gnome-terminal -- bash -c '%s; read -n 1 -p \"Appuyez sur une touche pour fermer le terminal...\"'", command);
            else                                             final_command = g_strdup_printf("gnome-terminal -- bash -c '%s'"                                                                    , command);
            g_spawn_command_line_async(final_command, NULL);
            shrink_intern_minal();
            g_free(directory);
        }
        if (branch_selected) g_free(branch_selected);
    }
}

void show_commit_dialog(GtkTreeView *treeview, GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Commit", GTK_WINDOW(commit_window), GTK_DIALOG_MODAL, "Valider", GTK_RESPONSE_ACCEPT, "Annuler", GTK_RESPONSE_CANCEL, NULL);
    // Largeur souhaitée de la fenêtre
    gtk_window_set_default_size(GTK_WINDOW(dialog), 800, -1);
    // Permettre le redimensionnement de la fenêtre
    gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *commit_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(commit_entry), "Entrez votre message de commit ici...");
    gtk_entry_set_width_chars(GTK_ENTRY(commit_entry), 40);
    gtk_container_add(GTK_CONTAINER(content_area), commit_entry);
    gtk_widget_show_all(content_area);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
    
    gint response  = gtk_dialog_run(GTK_DIALOG(dialog));
    if  (response == GTK_RESPONSE_ACCEPT) {
        const gchar *commit_message = gtk_entry_get_text(GTK_ENTRY(commit_entry));
        if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
            gchar *local_path = g_strdup_printf("/home/sdethyre/public_html/%s/%s", current_appli, branch_selected);
            gchar *commit_file_path = g_build_filename("/home/sdethyre/sandbox/dash_dev_boisson/commit_message.txt", NULL);
            FILE  *commit_file = fopen(commit_file_path, "w");
            if (commit_file) {
                fprintf(commit_file, "%s", commit_message);
                fclose(commit_file);
            }
            else {
                g_print("Failed to create commit message file.\n");
                g_free(local_path);
                g_free(commit_file_path);
                gtk_widget_destroy(dialog);
                return;
            }
            gchar *escaped_commit_message = g_shell_quote(commit_message);
            gchar *svn_command = g_strdup_printf("svn commit --file %s %s > /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", commit_file_path, local_path);

            FILE *svn_output = popen(svn_command, "r");
            if (svn_output) {
                char buffer[128];
                while (fgets(buffer, sizeof(buffer), svn_output) != NULL) printf("%s", buffer);
                int pclose_result = pclose(svn_output);
                if (pclose_result == 0) {
                    g_print("Commit réussi !\n");
                    // Read the output of svn_output.txt
                    char *line = NULL;
                    size_t buffer_size = 0;
                    ssize_t read;
                    gchar *to_copy = NULL;
                    FILE *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", "r");
                    if (file) {   
                        gchar *output_lines = g_strdup("");
                        while ((read = getline(&line, &buffer_size, file)) != -1) {
                            gchar *escaped_line  = escape_quotes(line);
                            gchar *cmd_show_line = g_strdup_printf("echo '%s'; ", escaped_line);
                            output_lines = g_strconcat(output_lines, cmd_show_line, NULL);
                            // Check if the line contains "Révision" and "propagée"
                            char *revision_str   = "Révision ";
                            char *propagated_str = " propagée.";
                            char *revision_start = strstr(line, revision_str);
                            char *propagated_end = strstr(line, propagated_str);
                            gchar *appli_name;
                            if (strcmp(current_appli, "openads") == 0) {
                                appli_name = "openfoncier";
                            }
                            if (strcmp(current_appli, "openaria") == 0) {
                                appli_name = "openaria";
                            }
                            if (strcmp(current_appli, "opencimetiere") == 0) {
                                appli_name = "opencimetiere";
                            }
                            if (strcmp(current_appli, "opendebitdeboisson") == 0) {
                                appli_name = "openboisson";
                            }
                            if (strcmp(current_appli, "openelec") == 0) {
                                appli_name = "openelec";
                            }
                            if (strcmp(current_appli, "openresultat") == 0) {
                                appli_name = "openresultat";
                            }
                            if (revision_start  != NULL && propagated_end != NULL) {
                                // Extract the part of the line containing the revision number
                                char *revision_number_start = revision_start + strlen(revision_str);
                                size_t revision_number_length = propagated_end - revision_number_start;
                                char *revision_number = (char *)malloc(revision_number_length + 1);
                                strncpy(revision_number, revision_number_start, revision_number_length);
                                revision_number[revision_number_length] = '\0';

                                int revision = atoi(revision_number);
                                printf("Numéro de révision : %d\n", revision);
                                // to_copy = g_strdup_printf("-r%d : %s", revision, commit_message);
                                to_copy = g_strdup_printf("- [%s](https://adullact.net/scm/viewvc.php/%s?view=revision&revision=%d)", commit_message, appli_name, revision);
// - [fix: methodes manage_finalizing d'instruction + treatment_document_sign d'instruction_modale (déplacement de variables)](https://adullact.net/scm/viewvc.php/openfoncier?view=revision&revision=19348)
                                free(revision_number);
                                fclose(file);
                                break;
                            }
                        }
                        gchar *escaped_to_copy = escape_quotes(to_copy);
                        // gchar *cmd_show_all_lines = g_strdup_printf("gnome-terminal -- bash -c \"cat /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt; echo '\n\n%s\n'; read -s -r -p 'Appuyez sur Entrée pour fermer le terminal...'\"", escaped_to_copy);
                        gchar *cmd_interne = g_strdup_printf("cat /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt; echo '\n\n%s\n'", escaped_to_copy);
                        grow_intern_minal();
                        execute_bash_intern(output_text_view, cmd_interne);

                        g_free(output_lines);
                        g_free(line);
                        // remove("/home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt");
                        g_free(escaped_to_copy);
                        g_free(to_copy);
                    }
                }
                else g_print("Failed to execute svn commit.\n");
            }
            else  g_print("Failed to execute svn commit.\n");
            g_free(local_path);
            g_free(commit_file_path);
            g_free(svn_command);
            g_free(escaped_commit_message);
        }
    }
    gtk_widget_destroy(dialog); 
}

void show_checkout_dialog(GtkTreeView *treeview, GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Checkout", GTK_WINDOW(checkout_window), GTK_DIALOG_MODAL, "Valider", GTK_RESPONSE_ACCEPT, "Annuler", GTK_RESPONSE_CANCEL, NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 800, -1);
    gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *checkout_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(checkout_entry), "Entrez le nom de branche à récupérer ici...");
    gtk_entry_set_width_chars(GTK_ENTRY(checkout_entry), 40);
    gtk_container_add(GTK_CONTAINER(content_area), checkout_entry);
    gtk_widget_show_all(content_area);
    
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const gchar *checkout_message   = gtk_entry_get_text(GTK_ENTRY(checkout_entry));
        gchar *escaped_checkout_message = g_shell_quote(checkout_message);
        gchar *appli_name;
        if (strcmp(current_appli, "openads") == 0) {
            appli_name = "openfoncier";
        }
        if (strcmp(current_appli, "openaria") == 0) {
            appli_name = "openaria";
        }
        if (strcmp(current_appli, "opencimetiere") == 0) {
            appli_name = "opencimetiere";
        }
        if (strcmp(current_appli, "opendebitdeboisson") == 0) {
            appli_name = "openboisson";
        }
        if (strcmp(current_appli, "openelec") == 0) {
            appli_name = "openelec";
        }
        if (strcmp(current_appli, "openresultat") == 0) {
            appli_name = "openresultat";
        }
        
        gchar *svn_command = g_strdup_printf("gnome-terminal -- bash -c 'cd /home/sdethyre/public_html/%s && svn checkout svn+ssh://sdethyre@scm.adullact.net/svn/%s/branches/%s'", current_appli, appli_name, escaped_checkout_message);
        printf("svn %s", svn_command);
        g_spawn_command_line_async(svn_command, NULL);
        g_free(svn_command);
        g_free(escaped_checkout_message);
    }
    gtk_widget_destroy(dialog); 
}

void delete_branch(GtkTreeView *treeview, GtkWidget *widget, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
         GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(widget),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Êtes-vous sûr de vouloir supprimer la branche %s?",
            branch_selected
        );

        gtk_window_set_title(GTK_WINDOW(dialog), "Confirmer Suppression");
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // User chose not to delete the branch, return without doing anything
        if (result != GTK_RESPONSE_YES) return;  
        
        gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s", current_appli);
        if (chdir(directory) != 0) {
            g_print("Impossible de changer de répertoire.\n");
            g_free(directory);
            return;
        }
        gchar *appli_name;
        if (strcmp(current_appli, "openads") == 0) {
            appli_name = "openfoncier";
        }
        if (strcmp(current_appli, "openaria") == 0) {
            appli_name = "openaria";
        }
        if (strcmp(current_appli, "opencimetiere") == 0) {
            appli_name = "opencimetiere";
        }
        if (strcmp(current_appli, "opendebitdeboisson") == 0) {
            appli_name = "openboisson";
        }
        if (strcmp(current_appli, "openelec") == 0) {
            appli_name = "openelec";
        }
        if (strcmp(current_appli, "openresultat") == 0) {
            appli_name = "openresultat";
        }
        gchar *svn_command = g_strdup_printf("svn delete svn+ssh://sdethyre@scm.adullact.net/svn/%s/branches/%s -m \"chore(branch): suppression de la branche de développement %s\" > /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", appli_name, branch_selected, branch_selected);

        FILE *svn_output = popen(svn_command, "r");
        if (svn_output) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), svn_output) != NULL) printf("%s", buffer);
            
            int pclose_result = pclose(svn_output);
            if (pclose_result == 0) {
                g_print("Delete réussi !\n");

                char   *line        = NULL;
                size_t  buffer_size = 0;
                ssize_t read;
                gchar  *to_copy     = NULL;
                FILE   *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", "r");
                if (file) {   
                    gchar *output_lines      = g_strdup("");
                    while ((read = getline(&line, &buffer_size, file)) != -1) {
                        gchar *escaped_line  = escape_quotes(line);
                        gchar *cmd_show_line = g_strdup_printf("echo '%s'; ", escaped_line);
                        output_lines = g_strconcat(output_lines, cmd_show_line, NULL);

                        // Check if the line contains "Révision" and "propagée"
                        char *revision_str   = "Révision ";
                        char *propagated_str = " propagée.";
                        char *revision_start = strstr(line, revision_str);
                        char *propagated_end = strstr(line, propagated_str);
                        if (revision_start != NULL && propagated_end != NULL) {
                            // Extract the part of the line containing the revision number
                            char *revision_number_start = revision_start + strlen(revision_str);
                            size_t revision_number_length = propagated_end - revision_number_start;
                            char *revision_number = (char *)malloc(revision_number_length + 1);
                            strncpy(revision_number, revision_number_start, revision_number_length);
                            revision_number[revision_number_length] = '\0';

                            int revision = atoi(revision_number);
                            printf("Numéro de révision : %d\n", revision);
                            gchar *delete_message = g_strdup_printf("chore(branch): suppression de la branche de développement %s", branch_selected);
                            // to_copy = g_strdup_printf("-r%d : %s", revision, delete_message);
                            to_copy = g_strdup_printf("- [%s](https://adullact.net/scm/viewvc.php/%s?view=revision&revision=%d)", delete_message, appli_name, revision);


                            free(revision_number);
                            fclose(file);
                            break;
                        }
                    }
                    gchar *escaped_to_copy = escape_quotes(to_copy);
                    gchar *cmd_interne = g_strdup_printf("cat /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt; echo '\n\n%s\n'", escaped_to_copy);
                    grow_intern_minal();
                    execute_bash_intern(output_text_view, cmd_interne);

                    g_free(output_lines);
                    g_free(line);
                    // remove("/home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt");
                    g_free(escaped_to_copy);
                    g_free(to_copy);
                }
            }
            else g_print("Failed to execute svn commit.\n");
        }
        else g_print("Failed to execute svn commit.\n");
        g_free(svn_command);
    }
}

gdouble calculate_mean(gdouble first_member, gdouble second_member){
    return ((first_member + second_member) / 2);
}

void select_row_at_index(GtkTreeView *treeview, int index) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    if (index >= 0) {
        GtkTreePath *path = gtk_tree_path_new_from_indices(index, -1);
        gtk_tree_selection_select_path(selection, path);
        gtk_tree_path_free(path);
    } 
   // else  printf("Index invalide");
}

static void on_popup_close_info(GtkWidget *widget, gpointer user_data) { 
    gtk_widget_destroy(popup_info); 
    gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));
}

static void on_popup_close_builds(GtkWidget *widget, gpointer user_data) { 
    gtk_widget_destroy(popup_builds);  
    gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));
}

static void on_popup_close_graph(GtkWidget *widget, gpointer user_data) { 
    gtk_widget_destroy(popup_graph);
    gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));
}

static void on_popup_destroy_info(GtkWidget *widget, gpointer user_data) { 
    popup_info= NULL; 
            gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));
}

static void on_popup_destroy_builds(GtkWidget *widget, gpointer user_data) { 
    popup_builds = NULL; 
            gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));
}

static void on_popup_destroy_graph(GtkWidget *widget, gpointer user_data) { 
    popup_graph = NULL;
            gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));
}

void create_line_popup(GtkWidget *box, gchar * script_name, gchar * branch_name, gchar * context){
    GtkWidget *output_label_left, *output_label_right;
    GString   *output = g_string_new(NULL);  
    GtkWidget *labels_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); // Conteneur horizontal
    gtk_widget_set_halign(labels_box, GTK_ALIGN_CENTER);
    if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/moyenne.py") == 0 ) {
        gchar *output_label_tmp = g_strdup("\t\t\t (moyenne)\n");
        output_label_left = gtk_label_new(output_label_tmp);
        gchar *output_label_tmp_2 =  g_strdup_printf("\t    %d %%\n", (gint)calculate_mean(percent_by_time, percent_by_lines));
        output_label_right = gtk_label_new(output_label_tmp_2);
        element_neon_custom_on(GTK_WIDGET(output_label_right), "orange");
        g_free(output_label_tmp);
        GtkWidget *spacer = gtk_label_new(""); // Label vide comme espacement
        gtk_widget_set_visible(spacer, FALSE); // Rendu invisible

        gtk_box_pack_start(GTK_BOX(labels_box), output_label_left , FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(labels_box), spacer            , FALSE, FALSE, 0); // Espacement horizontal
        gtk_box_pack_start(GTK_BOX(labels_box), output_label_right, FALSE, FALSE, 0);

        gtk_box_pack_start(GTK_BOX(box), labels_box, TRUE, TRUE, 0);
        g_string_free(output, TRUE);
        return;
    }
    if (strcmp(script_name, "Job inexistant : erreur 404") == 0 ) {
        gchar *output_label_tmp   = g_strdup_printf("Job inexistant : ");
        output_label_left         = gtk_label_new(output_label_tmp);
        output_label_right        = gtk_label_new("erreur 404");
        element_neon_custom_on(GTK_WIDGET(output_label_right), "red");
        g_free(output_label_tmp);
        GtkWidget *spacer = gtk_label_new(""); // Label vide comme espacement
        gtk_widget_set_visible(spacer, FALSE); // Rendu invisible

        gtk_box_pack_start(GTK_BOX(labels_box), output_label_left , FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(labels_box), spacer            , FALSE, FALSE, 0); // Espacement horizontal
        gtk_box_pack_start(GTK_BOX(labels_box), output_label_right, FALSE, FALSE, 0);

        gtk_box_pack_start(GTK_BOX(box), labels_box, TRUE, TRUE, 0);
        g_string_free(output, TRUE);
        return;
    }
    gchar *command = g_strdup_printf("python3.10 %s %s %s", script_name, current_appli, branch_name);
    FILE *fp = popen(command, "r");
    if (fp == NULL) g_free(command);
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) g_string_append(output, buffer);
    pclose(fp);
    g_free(command);
            g_strstrip(output->str);

    if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/count_display_fails_d.py") == 0 || strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/fails_d.py") == 0) {
        gchar *output_label_tmp = NULL;
        if (strcmp(output->str, "0") == 0) {
            output_label_left  = gtk_label_new("Aucun");
            element_neon_custom_on(GTK_WIDGET(output_label_left), "green");
            output_label_right = gtk_label_new(" fail");
        }
        else if (strcmp(output->str, "1") == 0) {
            output_label_tmp   = g_strdup("Nombre de fail : ");
            output_label_left  = gtk_label_new(output_label_tmp);
            output_label_right = gtk_label_new("1");
            element_neon_custom_on(GTK_WIDGET(output_label_right), "red");
        }
        else if (strcmp(output->str, "🔴") == 0) {
            output_label_tmp   = g_strdup("Dernier job ");
            output_label_left  = gtk_label_new(output_label_tmp);
            output_label_right = gtk_label_new("incomplet");
            element_neon_custom_on(GTK_WIDGET(output_label_right), "red");
        }
        else if (strcmp(output->str, "🟡") == 0) {
            output_label_tmp   = g_strdup("Job ");
            output_label_left  = gtk_label_new(output_label_tmp);
            output_label_right = gtk_label_new("en cours");
            element_neon_custom_on(GTK_WIDGET(output_label_right), "orange");
        }
        else {
            output_label_left  = gtk_label_new("Nombre de fails : ");

            output_label_tmp   = g_strdup(output->str);
            gchar *output_label_tmp_2 = g_strdup_printf("%s",output_label_tmp);
            output_label_right = gtk_label_new(output_label_tmp_2);
            element_neon_custom_on(GTK_WIDGET(output_label_right), "red");
        }
        g_free(output_label_tmp);
    }
    else if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/last_test_d.py") == 0 ) {
        if (strcmp(context, "🔴") == 0) {
            gchar *output_label_tmp   = g_strdup("Dernier test : ");
            output_label_left  = gtk_label_new(output_label_tmp);
            gchar *output_label_tmp_2 = g_strdup_printf("%s",output->str);
            output_label_right = gtk_label_new(output_label_tmp_2);
            element_neon_custom_on(GTK_WIDGET(output_label_right), "orange");
            g_free(output_label_tmp);
            g_free(output_label_tmp_2);
        }
        else if (strcmp(context, "🟡") == 0) {
            gchar *output_label_tmp   = g_strdup("Test en cours : ");
            output_label_left  = gtk_label_new(output_label_tmp);
            gchar *output_label_tmp_2 = g_strdup_printf("%s",output->str);
            output_label_right = gtk_label_new(output_label_tmp_2);
            element_neon_custom_on(GTK_WIDGET(output_label_right), "orange");
            g_free(output_label_tmp);
            g_free(output_label_tmp_2);
        }
    }
    else if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/percent_d.py") == 0 ) {
        gchar *output_label_tmp   = g_strdup("\nAvancement (timestamp)         ");
        output_label_left         = gtk_label_new(output_label_tmp);
        gchar *output_label_tmp_2 = g_strdup_printf("\n %s %%", output->str);
        output_label_right        = gtk_label_new(output_label_tmp_2);
        g_free(output_label_tmp);
        g_free(output_label_tmp_2);
    }
    else if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/percent_by_line_d.py") == 0 ) {
        gchar *output_label_tmp   = g_strdup("\t\t\t\t   (lignes)");
        output_label_left         = gtk_label_new(output_label_tmp);
        gchar *output_label_tmp_2 = g_strdup_printf("\t       %s %%", output->str);
        output_label_right        = gtk_label_new(output_label_tmp_2);
        g_free(output_label_tmp);
        g_free(output_label_tmp_2);
    }
    else if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/build_creator_d.py") == 0 ) {
        gchar *output_label_tmp   = g_strdup_printf("%s\n",output->str);
        output_label_left         = gtk_label_new(output_label_tmp);
        output_label_right        = gtk_label_new("\n");
        g_free(output_label_tmp);
    }
    else {
        output_label_left  = gtk_label_new(output->str);
        output_label_right = gtk_label_new("");
    }

    if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/percent_d.py")         == 0 ) percent_by_time = strtod(output->str, NULL);
    if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/percent_by_line_d.py") == 0 ) percent_by_lines = strtod(output->str, NULL);

    GtkWidget *spacer = gtk_label_new(""); // Label vide comme espacement
    gtk_widget_set_visible(spacer, FALSE); // Rendu invisible

    gtk_box_pack_start(GTK_BOX(labels_box), output_label_left, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(labels_box), spacer, FALSE, FALSE, 0); // Espacement horizontal
    gtk_box_pack_start(GTK_BOX(labels_box), output_label_right, FALSE, FALSE, 0);

    if (strcmp(output->str, "") == 0) return;
    
    gtk_box_pack_start(GTK_BOX(box), labels_box, TRUE, TRUE, 0);
    g_string_free(output, TRUE);
}

void create_builds_list() {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;
            gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    // gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_dev:");
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;
    gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);

    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
        gchar *titre = g_strdup_printf("Builds Jenkins sur %s", branch_selected);
        popup_builds = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(popup_builds), titre);
        gtk_window_set_position(GTK_WINDOW(popup_builds), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(popup_builds), 600, 450);
        GtkWidget *popup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(popup_builds), popup_box);

        gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s/", current_appli);
        gchar file_path[512];
        snprintf(file_path, sizeof(file_path), "%s%s", directory, branch_selected);

        gchar *command_free_count_builds_fails = g_strdup_printf(" echo '' > /home/sdethyre/sandbox/dash_dev_boisson/last_builds_results");
        system(command_free_count_builds_fails);
        g_free(command_free_count_builds_fails);

        gchar *command_populate_output = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/count_last_builds_fails.py %s %s", current_appli, branch_selected);
        system(command_populate_output);
        g_free(command_populate_output);

        FILE *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results", "r");
        if (file == NULL) {
            perror("Erreur lors de l'ouverture du fichier");
            g_free(titre);
            return;
        }

        GString *all_builds_fails = g_string_sized_new(1024);  // Allouer une taille pré-estimée pour éviter des réallocations fréquentes
        char line[1024];
        int line_count = count_lines_in_file("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results");
        int i = line_count;

        while (fgets(line, sizeof(line), file) != NULL && i >= 0) {
            int build_number;
            // int error_count;
            gchar fails_info[100];
            int float_fails_info;

            // if (sscanf(line, "Build %d - errors: %d, fails: %99[^\n]", &build_number, &error_count, fails_info) == 3) {
            //     float_fails_info = atoi(fails_info);
            //     y_values[i] = (float_fails_info >= INT_MIN && float_fails_info <= INT_MAX) ? float_fails_info : 0;
            // }
            // char info[1024];
            // if (sscanf(line, "Build %d - errors: %d, fails: %[^\n]", &build_number, &error_count, info) >= 2) {
            //     g_string_append_printf(all_builds_fails, "       %d\t\t\t\t\t %d\t\t\t\t      %s\n\n", build_number, error_count, fails_info);
            // }

            // if (sscanf(line, "Build %d - errors: %d, fails: %99[^\n]", &build_number, &error_count, fails_info) >= 3) {
            if (sscanf(line, "Build %d, fails: %99[^\n]", &build_number, fails_info) >= 2) {
                float_fails_info = atoi(fails_info);
                y_values[i] = (float_fails_info >= INT_MIN && float_fails_info <= INT_MAX) ? float_fails_info : 0;

                gchar info[1024];
                g_snprintf(info, sizeof(info), "\t       %d\t\t\t  %s\t   \n\n", build_number, fails_info);
                g_string_append(all_builds_fails, info);
            }
            --i;
        }
        fclose(file);

        GtkWidget *column_names = gtk_label_new(NULL);
        gtk_label_set_xalign(GTK_LABEL(column_names), 0.5);
        gchar *all_builds_fails_tmp = g_strdup_printf("<b>\n\tNuméro\t\t       Fails\t\t     Date \t\t\t  Heures\n\n\n</b>%s", all_builds_fails->str);
        gtk_label_set_markup(GTK_LABEL(column_names), all_builds_fails_tmp);  // Markup pour afficher le texte formaté
        gtk_widget_set_margin_top(column_names, 10);

        gtk_box_pack_start(GTK_BOX(popup_box), column_names, TRUE, TRUE, 0);
        g_string_free(all_builds_fails, TRUE);

        GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        GtkWidget *result_label = gtk_label_new("🔴 : incomplet\t\t\t🟡 : en cours\n\n");
        gtk_widget_set_margin_top(result_label, 10);

        // Utilisez une boîte verticale pour disposer le label et le bouton.
        GtkWidget *vertical_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_pack_start(GTK_BOX(vertical_box), result_label, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(vertical_box), button_box, TRUE, TRUE, 0);

        gtk_box_pack_start(GTK_BOX(popup_box), vertical_box, TRUE, TRUE, 0);

        GtkWidget *close_button = gtk_button_new_with_label("Fermer");
        g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(on_popup_close_builds), NULL);
        gtk_container_add(GTK_CONTAINER(button_box), close_button);

        g_signal_connect(G_OBJECT(popup_builds), "destroy", G_CALLBACK(on_popup_destroy_builds), NULL);
        gtk_widget_show_all(popup_builds);

        g_free(titre);

    }
}

void create_graph_fails() {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;
    gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    // gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_dev:");
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
    

    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
        gchar *titre = g_strdup_printf("Builds Jenkins sur %s", branch_selected);
        
        popup_graph = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(popup_graph), titre);
        gtk_window_set_position(GTK_WINDOW(popup_graph), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(popup_graph), 600, 500);

        GtkWidget *popup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(popup_graph), popup_box);

        gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s/", current_appli);
        gchar file_path[512];
        snprintf(file_path, sizeof(file_path), "%s%s", directory, branch_selected);

        gchar *command_free_count_builds_fails = g_strdup_printf(" echo '' > /home/sdethyre/sandbox/dash_dev_boisson/last_builds_results");
        system(command_free_count_builds_fails);
        g_free(command_free_count_builds_fails);

        gchar *command_populate_output = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/count_last_builds_fails.py %s %s", current_appli, branch_selected);
        system(command_populate_output);
        g_free(command_populate_output);

        FILE *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results", "r");
        if (file == NULL) {
            perror("Erreur lors de l'ouverture du fichier");
            g_free(titre);  // Libération de la mémoire allouée
            return;
        }

        char line[1024];
        int line_count = count_lines_in_file("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results");
        int i = line_count;  // Utilisation directe de line_count au lieu de recalculer
        while (fgets(line, sizeof(line), file) != NULL && i >= 0) {
            int build_number;
            int error_count;
            gchar fails_info[100];
            int float_fails_info;
            if (sscanf(line, "Build %d - errors: %d, fails: %99[^\n]", &build_number, &error_count, fails_info) == 3) {
                float_fails_info = atoi(fails_info);
                y_values[i] = (float_fails_info >= INT_MIN && float_fails_info <= INT_MAX) ? float_fails_info : 0;
                printf("y:%d", y_values[i]);
            }
            --i;
        }
        fclose(file);

        GtkWidget *da = gtk_drawing_area_new();
        gtk_widget_set_size_request(da, 200, 400);
        gtk_container_add(GTK_CONTAINER(popup_box), da);
        g_signal_connect(G_OBJECT(da), "draw", G_CALLBACK(on_draw_graph), NULL);

        GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start(GTK_BOX(popup_box), button_box, TRUE, TRUE, 0);

        GtkWidget *close_button = gtk_button_new_with_label("Fermer");
        g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(on_popup_close_graph), NULL);
        gtk_container_add(GTK_CONTAINER(button_box), close_button);

        g_signal_connect(G_OBJECT(popup_graph), "destroy", G_CALLBACK(on_popup_destroy_graph), NULL);
        gtk_widget_show_all(popup_graph);

        g_free(titre);  // Libération de la mémoire allouée pour 'titre'
    }
}

// Fonction pour mettre à jour committed_label en fonction de la valeur sélectionnée.
void update_commit_label(GtkLabel *committed_label, const gchar *branch_selected, int selected_value) {

    gchar *appli_name;
    if (strcmp(current_appli, "openads") == 0) {
        appli_name = "openfoncier";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        appli_name = "openaria";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        appli_name = "opencimetiere";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        appli_name = "openboisson";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        appli_name = "openelec";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        appli_name = "openresultat";
    }
    // gchar *command_tmp = g_strdup_printf("svn log -r HEAD:1 --limit %d svn+ssh://sdethyre@scm.adullact.net/svn/openboisson/branches/%s | sed '/^r[0-9]\{5\}/s/^/\n/'", selected_value, branch_selected);
    gchar *command_tmp = g_strdup_printf("svn log -r HEAD:1 --limit %d svn+ssh://sdethyre@scm.adullact.net/svn/%s/branches/%s | sed '/^r[0-9]\\{5\\}/s/^/\\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\\n/'", selected_value, appli_name, branch_selected);
    gchar *commit_message = execute_command_custom(command_tmp);
    gchar *committed_full;

    if (commit_message == NULL)
        committed_full = g_strdup_printf("Branche inexistante");
    else
        committed_full = g_strdup_printf("\t%s", commit_message);

    gtk_label_set_text(committed_label, committed_full);

    // Libérez la mémoire allouée.
    g_free(command_tmp);
    g_free(commit_message);
    g_free(committed_full);
}

// Gestionnaire de signal pour le changement de sélection dans combo_box.
void on_combo_box_changed(GtkComboBox *combo_box, gpointer user_data) {
    int new_selected_value = gtk_combo_box_get_active(combo_box) + 1;
    // Variable globale branch_selected_l
    update_commit_label(GTK_LABEL(user_data), branch_selected_l, new_selected_value);
}

void create_local_info(GtkTreeView *treeview, gboolean show_last_commit) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;
    gint width = 600;
    save_current_dev(treeview);
    gint nb_commits_available = 20;
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
    branch_selected_l = branch_selected;
    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
        popup_info = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gchar *titre = g_strdup_printf("Local - %s", branch_selected);
        gtk_window_set_title(GTK_WINDOW(popup_info), titre);
        gtk_window_set_position(GTK_WINDOW(popup_info), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(popup_info), width, 600);
        GtkWidget *popup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(popup_info), popup_box);

        gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s/", current_appli);
        gchar file_path[512];
        snprintf(file_path, sizeof(file_path), "%s%s", directory, branch_selected);

        gchar *modified; 
        int count = count_modified_files(file_path);
        if (count < 1) modified = g_strdup_printf("Aucun fichier modifié en local"); 
        else if (count < 2) modified = g_strdup_printf("Modifié en local : %d", count); 
        else modified = g_strdup_printf("Modifiés en local : %d", count); 
        GtkWidget *modified_label = gtk_label_new(modified);
        gtk_widget_set_margin_top(modified_label, 20); 
        gtk_box_pack_start(GTK_BOX(popup_box), modified_label, TRUE, TRUE, 0);

        gchar *directory_file = g_strdup_printf("%s%s", directory, branch_selected);
        gchar *modified_files = list_modified_files(directory_file);
        g_strstrip(modified_files);
        g_strchomp(modified_files);
        if (strcmp(modified_files, "") != 0){
            GtkWidget *modified_labels_list = gtk_label_new(modified_files);
            // gtk_widget_set_margin_top(modified_labels_list, 20); 
            gtk_label_set_line_wrap(GTK_LABEL(modified_labels_list), TRUE);
            gtk_label_set_line_wrap_mode(GTK_LABEL(modified_labels_list), PANGO_WRAP_WORD_CHAR);
            gtk_box_pack_start(GTK_BOX(popup_box), modified_labels_list, TRUE, TRUE, 0);
        }
        GtkWidget *button_box   = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
        gtk_box_pack_start(GTK_BOX(popup_box), button_box, TRUE, TRUE, 0);

        GtkWidget *close_button = gtk_button_new_with_label("Fermer");
        g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(on_popup_close_info), NULL);
        gtk_container_add(GTK_CONTAINER(button_box), close_button);
        GtkWidget *committed_label;
        if (show_last_commit) {
            gchar *appli_name;
            if (strcmp(current_appli, "openads") == 0) {
                appli_name = "openfoncier";
            }
            if (strcmp(current_appli, "openaria") == 0) {
                appli_name = "openaria";
            }
            if (strcmp(current_appli, "opencimetiere") == 0) {
                appli_name = "opencimetiere";
            }
            if (strcmp(current_appli, "opendebitdeboisson") == 0) {
                appli_name = "openboisson";
            }
            if (strcmp(current_appli, "openelec") == 0) {
                appli_name = "openelec";
            }
            if (strcmp(current_appli, "openresultat") == 0) {
                appli_name = "openresultat";
            }
            gchar *command_tmp = g_strdup_printf("svn log -r HEAD:1 --limit 1 svn+ssh://sdethyre@scm.adullact.net/svn/%s/branches/%s", appli_name, branch_selected);
            gchar *commit_message = execute_command_custom(command_tmp);
            gchar *committed_full;
            
            if (commit_message == NULL) committed_full = g_strdup_printf("Branche inexistante");
            else                        committed_full = g_strdup_printf("%s", commit_message);     

            GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
            gtk_widget_set_size_request(scroll_window, -1, 200); // Taille scrollable

            committed_label = gtk_label_new(committed_full);
            gtk_label_set_line_wrap(GTK_LABEL(committed_label), TRUE);
            gtk_label_set_line_wrap_mode(GTK_LABEL(committed_label), PANGO_WRAP_WORD_CHAR);

            gtk_container_add(GTK_CONTAINER(scroll_window), committed_label);
            gtk_box_pack_start(GTK_BOX(popup_box), scroll_window, TRUE, TRUE, 0);
        }

        GtkWidget *combo_box = gtk_combo_box_text_new();
        for (int i = 1; i <= nb_commits_available; i++) {
            gchar *value_str = g_strdup_printf("%d", i);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), value_str);
            g_free(value_str);
        }
        g_signal_connect(G_OBJECT(combo_box), "changed", G_CALLBACK(on_combo_box_changed), committed_label);
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0); // Par défaut, 1ère entrée
        gtk_box_pack_start(GTK_BOX(button_box), combo_box, FALSE, FALSE, 0);

         GtkWidget *commit_button = gtk_button_new_with_label("Commits !");
        g_signal_connect(G_OBJECT(commit_button), "clicked", G_CALLBACK(on_popup_close_info), NULL);
        gtk_container_add(GTK_CONTAINER(button_box), commit_button);
        
        g_signal_connect(G_OBJECT(popup_info), "destroy", G_CALLBACK(on_popup_destroy_info), NULL);
        gtk_widget_show_all(popup_info);
    }
}

void create_job(GtkTreeView *treeview, GtkWidget *widget) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;
    // GtkWidget *widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
         GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(widget),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Êtes-vous sûr de vouloir créer un nouveau build sur le job %s?",
            branch_selected
        );

        gtk_window_set_title(GTK_WINDOW(dialog), "Nouveau build");
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if (result != GTK_RESPONSE_YES) return; 
        
        gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s", current_appli);
        if (chdir(directory) != 0) {
            g_print("Impossible de changer de répertoire.\n");
            g_free(directory);
            return;
        }

        gchar *svn_command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/create_build.py %s %s > /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", current_appli, branch_selected);

        FILE *svn_output = popen(svn_command, "r");
        if (svn_output) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), svn_output) != NULL) printf("%s", buffer);
            
            int pclose_result = pclose(svn_output);
            if (pclose_result == 0) {
                g_print("Build créé !\n");

                char   *line        = NULL;
                size_t  buffer_size = 0;
                ssize_t read;
                // gchar  *to_copy     = NULL;
                FILE   *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", "r");
                if (file) {   
                    gchar *output_lines      = g_strdup("");
                    while ((read = getline(&line, &buffer_size, file)) != -1) {
                        gchar *escaped_line  = escape_quotes(line);
                        gchar *cmd_show_line = g_strdup_printf("echo '%s'; ", escaped_line);
                        output_lines = g_strconcat(output_lines, cmd_show_line, NULL);
                    }
                    gchar *cmd_interne = g_strdup_printf("cat /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt");
                    grow_intern_minal();
                    execute_bash_intern(output_text_view, cmd_interne);

                    g_free(output_lines);
                    g_free(line);
                    // remove("/home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt");
                }
            }
            else g_print("Failed to execute create build.\n");
        }
        else g_print("Failed to execute create build.\n");
        g_free(svn_command);
    }
}

void on_create_build_clicked(GtkButton *button, gpointer user_data) {
    // Appel de create_job avec NULL comme argument widget
    create_job((GtkTreeView *)user_data, NULL);
}

void create_jenkins_info(GtkTreeView *treeview, gboolean show_last_commit) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;
    gint width = 600;
    save_current_dev(treeview);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);

    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
        popup_info = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gchar *titre = g_strdup_printf("Job - %s", branch_selected);
        gtk_window_set_title(GTK_WINDOW(popup_info), titre);
        gtk_window_set_position(GTK_WINDOW(popup_info), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(popup_info), width, 300);
        GtkWidget *popup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(popup_info), popup_box);

        gchar *command_populate_last_build = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/get_last_build_api_results.py %s %s", current_appli, branch_selected);
        system(command_populate_last_build);
        g_free(command_populate_last_build);

        // int status_last_build;
        // wait(&status_last_build);
        
        gchar *command_populate_output = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/output_jenkins_to_file.py %s %s", current_appli, branch_selected);
        system(command_populate_output);
        g_free(command_populate_output);

        // int status_output;
        // wait(&status_output);
        
        gchar *command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/fails.py %s %s", current_appli, branch_selected);
        FILE *fp = popen(command, "r");
        if (fp == NULL) {
            g_free(command);
        }
        char buffer[128];
        GString *output = g_string_new(NULL);  
        while (fgets(buffer, sizeof(buffer), fp) != NULL) g_string_append(output, buffer);
        pclose(fp);
        g_free(command);
        g_strstrip(output->str);    // Supprimer les espaces de début et de fin
        g_strchomp(output->str);
        if (strcmp(output->str, "Job inexistant : erreur 404") == 0) {
            gtk_window_set_default_size(GTK_WINDOW(popup_info), width, 200);
            create_line_popup(popup_box, "Job inexistant : erreur 404"                           , branch_selected, "404");
        }
        else if (strcmp(output->str, "🔴") == 0) {
            gtk_window_set_default_size(GTK_WINDOW(popup_info), width, 450);
            // GtkWidget *inter_line = gtk_label_new("━━━━━━━━━━");  
            // gtk_box_pack_start(GTK_BOX(popup_box), inter_line, TRUE, TRUE, 0);
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/fails_d.py"              , branch_selected, "🔴");                
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/date_hour_d.py"          , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/build_creator_d.py"      , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/last_test_d.py"          , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/count_display_fails_d.py", branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_d.py"            , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_by_line_d.py"    , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/moyenne.py"              , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/authors_d.py"            , branch_selected, "🔴");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/changes_jenkins_d.py"    , branch_selected, "🔴");
        }
        else if (strcmp(output->str, "🟡") == 0) {
            gtk_window_set_default_size(GTK_WINDOW(popup_info), width, 600);
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/fails_d.py"              , branch_selected, "🟡");                
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/date_hour_d.py"          , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/build_creator_d.py"      , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/last_test_d.py"          , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/count_display_fails_d.py", branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_d.py"            , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_by_line_d.py"    , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/moyenne.py"              , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/authors_d.py"            , branch_selected, "🟡");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/changes_jenkins_d.py"    , branch_selected, "🟡");
        }
        else {
            gtk_window_set_default_size(GTK_WINDOW(popup_info), width, 450);
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/fails_d.py"              , branch_selected, "");                
            GtkWidget *output_label_3 = gtk_label_new("Dernier job achevé à 100 %\n");
            gtk_box_pack_start(GTK_BOX(popup_box), output_label_3, TRUE, TRUE, 0);
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/date_hour_d.py"          , branch_selected, "");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/build_creator_d.py"      , branch_selected, "");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/authors_d.py"            , branch_selected, "");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/changes_jenkins_d.py"    , branch_selected, "");
        }

        g_string_free(output, TRUE);

        GtkWidget *button_box   = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_box_pack_start(GTK_BOX(popup_box), button_box, TRUE, TRUE, 0);

        GtkWidget *close_button = gtk_button_new_with_label("Fermer");
        g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(on_popup_close_info), NULL);
        gtk_container_add(GTK_CONTAINER(button_box), close_button);
        
        GtkWidget *builds_button = gtk_button_new_with_label("Builds");
        g_signal_connect(G_OBJECT(builds_button), "clicked", G_CALLBACK(create_builds_list), treeview);
        gtk_container_add(GTK_CONTAINER(button_box), builds_button);
        
        GtkWidget *graph_button = gtk_button_new_with_label("Graph");
        g_signal_connect(G_OBJECT(graph_button), "clicked", G_CALLBACK(create_graph_fails), treeview);
        gtk_container_add(GTK_CONTAINER(button_box), graph_button);

        g_signal_connect(G_OBJECT(popup_info), "destroy", G_CALLBACK(on_popup_destroy_info), NULL);
        gtk_widget_show_all(popup_info);
    }
}

gboolean on_treeview_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

void on_menu_item_activate_L(GtkMenuItem *menu_item, gpointer user_data) {
    MenuItemData *data          = (MenuItemData *)user_data;
    gint selected_option_index  = data->index;
    GtkTreeView *treeview       = data->treeview;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    switch (selected_option_index) {
        case 0:
            open_web_page(treeview, user_data, "open_job.py", current_appli, combobox);
            generic_quit (treeview, pidFilename);
            break;
        case 1:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_jenkins_info(treeview, show_last_commit);
            break;
        case 2:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *branch_selected;
                gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
                if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
                    gchar *fails = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/display_fails.py %s %s", current_appli, branch_selected);
                    launch_terminal(treeview, fails, "intern_minal", "");
                }
            }
            break;
        case 3:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *branch_selected;
                gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
                if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
                    gchar *fails = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/output_jenkins.py %s %s", current_appli, branch_selected);
                    launch_terminal(treeview, fails, "intern_minal", "");
                }
            }
            break;
        case 4:
            open_web_page(treeview, user_data, "open_job_report.py", current_appli, combobox);
            generic_quit (treeview, pidFilename);
            break;
        case 5:
            launch_file(treeview, "/home/sdethyre/bin/atreal/./testLogs", current_appli);
            generic_quit (treeview, pidFilename);
            break;
        case 6:
            // launch_terminal(treeview, "tail -f /var/log/apache2/error.log", "return_key_title", "Apache");
            launch_terminal(treeview, "less /var/log/apache2/error.log", "return_key_title", "Apache");
            launch_terminal(treeview, "expect /home/sdethyre/sandbox/dash_dev_boisson/launch_postg_log.exp", "return_key", "Postgres");
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *branch_selected;
                gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
                if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
                    gchar *logFilePath = g_strdup_printf("/home/sdethyre/public_html/%s/%s/var/log/error.log", current_appli, branch_selected);
                    if (g_file_test(logFilePath, G_FILE_TEST_EXISTS)) {
                        // Le fichier existe, vous pouvez l'ouvrir
                        gchar *command = g_strdup_printf("tail -f %s", logFilePath);
                        launch_terminal(treeview, command, "return_key_title", "Var Log");
                        g_free(command);
                    } else {
                        // Le fichier n'existe pas, gestion de l'erreur ici
                        g_print("Le fichier n'existe pas : %s\n", logFilePath);
                    }
                }
            }
            break;
        case 7:
            open_web_page(treeview, user_data, "open_ti.py" , current_appli, combobox);
            generic_quit (treeview, pidFilename);
            break;
        default:
            break;
    }
    element_neon_off(GTK_WIDGET(label7));
}

void on_menu_item_activate_R(GtkMenuItem *menu_item, gpointer user_data) {
    MenuItemData *data          = (MenuItemData *)user_data;
    gint selected_option_index  = data->index;
    GtkTreeView *treeview       = data->treeview;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    switch (selected_option_index) {
        case 0:
            launch_file(treeview, "meld .", current_appli);
            generic_quit (treeview, pidFilename);
            break;
        case 1:
            // launch_terminal(treeview, "svn status --ignore-externals | grep '^M'| sed 's/^M//'", "intern_minal");
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_local_info(treeview, show_last_commit);
            break;
        case 2:
            show_commit_dialog(treeview, NULL, NULL);
            break;
        case 3:
            delete_branch(treeview, NULL, NULL);
            break;
        case 4:
            launch_terminal(treeview, "svn st --ignore-externals", "intern_minal", "");
            break;
        case 5:
            launch_terminal(treeview, "svn diff", "return_key", "");
            break;
        case 6:
            char *rewind_months = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "gource_rewind_months:");
            int rewind_months_int = atoi(rewind_months);
            char *rewind_days = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "gource_rewind_days:");
            int rewind_days_int = atoi(rewind_days);
            struct tm adjustedDate = adjustDate(rewind_months_int, rewind_days_int);
            printf("Année : %d, Mois : %02d, Jour : %02d\n", adjustedDate.tm_year, adjustedDate.tm_mon, adjustedDate.tm_mday);

            char gourceCommand[100];
            strftime(gourceCommand, sizeof(gourceCommand), "gource --start-date '%Y-%m-%d 00:00:00'", &adjustedDate);
            gchar *gourceCommandStr = g_strdup(gourceCommand);
            launch_file(treeview, gourceCommandStr, current_appli);
            break;
        case 7:
            show_checkout_dialog(treeview, NULL, NULL);
            break;
        case 8:
            launch_terminal(treeview, "svn update", "intern_minal" ,"");
            break;
        default:
            break;
    }
    element_neon_off(GTK_WIDGET(label9));
}

void on_menu_item_activate_B(GtkMenuItem *menu_item, gpointer user_data) {
    MenuItemData *data          = (MenuItemData *)user_data;
    gint selected_option_index  = data->index;
    GtkTreeView *treeview       = data->treeview;
    // GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    // GtkTreeModel *model;
    // GtkTreeIter iter;
    switch (selected_option_index) {
        case 0:
            open_web_page(treeview, user_data, "open_app.py", current_appli, combobox);
            generic_quit (treeview, pidFilename);
            break;
        case 1:
            launch_terminal(treeview, "", "merge","");
            break;
        case 2:
            launch_terminal(treeview, "", "meld", "");
            break;
        case 3:
            gchar *command = g_strdup_printf("cd /var/www/%s/locales/fr_FR/LC_MESSAGES && poedit openmairie.po", current_appli);
            launch_terminal(treeview, command, "enter_key", "");
            generic_quit (treeview, pidFilename);
            break;
        case 4:
            launch_file(treeview, "/home/sdethyre/sandbox/auto_boisson/./dev_boisson", current_appli);
            break;
        case 5:
            open_web_page(treeview, user_data, "open_adminer.py", current_appli, combobox);
            generic_quit (treeview, pidFilename);
            break;
        default:
            break;
    }
    element_neon_off(GTK_WIDGET(label8));
}

void on_menu_item_activate_V(GtkMenuItem *menu_item, gpointer user_data) {
    MenuItemData *data          = (MenuItemData *)user_data;
    gint selected_option_index  = data->index;
    GtkTreeView *treeview       = data->treeview;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    switch (selected_option_index) {
        case 0:
            open_web_page(treeview, user_data, "open_job.py", current_appli, combobox);
            break;
        case 1:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_jenkins_info(treeview, show_last_commit);
            break;
        case 2:
            launch_file(treeview, "meld .", current_appli);
            break;
        case 3:
            open_web_page(treeview, user_data, "open_ti.py" , current_appli, combobox);
            break;
        case 4:
            launch_file(treeview, "/home/sdethyre/bin/atreal/./testLogs", current_appli);
            break;
        case 5:
            make_label_blink (neon_blink_timeout_id_n, label5);
            launch_file(treeview, "/home/sdethyre/sandbox/auto_boisson/./dev_boisson", current_appli);
            break;
        case 6:
            // Exit
            break;
        case 7:
            launch_terminal(treeview, "svn update", "intern_minal", "");
            break;
        case 8:
            show_commit_dialog(treeview, NULL, NULL);
            break;
        case 9:
            launch_terminal(treeview, "", "merge", "");
            break;
        case 10:
            // launch_terminal(treeview, "svn status --ignore-externals | grep '^M'| sed 's/^M//'", "intern_minal");
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_local_info(treeview, show_last_commit);
            break;
        case 11:
            launch_terminal(treeview, "svn st --ignore-externals", "intern_minal", "");
            break;
        case 12:
            launch_terminal(treeview, "svn diff", "return_key", "");
            break;
        case 13:
            delete_branch(treeview, NULL, NULL);
            break;
        default:
            break;
    }
}

static gboolean on_right_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY) {
        // const gchar *menu_items[] = {"Option 1", "Option 2", "Option 3", "Option 4"};
        // const gchar *menu_items[] = {};
        GtkTreeView *treeview     = GTK_TREE_VIEW(widget);
        // Obtenir les coordonnées du curseur au moment du clic droit
        gdouble x, y;
        gdk_event_get_coords((GdkEvent*)event, &x, &y);
        gint cx = (gint)x, cy = (gint)y;

        // Convertir les coordonnées du curseur en coordonnées de la fenêtre du widget treeview
        GdkWindow *window = gtk_tree_view_get_bin_window(treeview);
        gint win_x, win_y;
        gdk_window_get_origin(window, &win_x, &win_y);
        cx -= win_x;
        cy -= win_y;
        // Obtenir le chemin de la ligne (élément) sous le curseur
        GtkTreePath *path = NULL;
        GtkTreeViewColumn *column = NULL;
        gint cell_x, cell_y; // Ajout des coordonnées du GtkCellRenderer
        gtk_tree_view_get_path_at_pos(treeview, cx, cy, &path, &column, &cell_x, &cell_y);

        // GtkWidget *menu = gtk_menu_new();
        // for (int i = 0; i < G_N_ELEMENTS(menu_items); i++) {
        //     GtkWidget *menu_item = gtk_menu_item_new_with_label(menu_items[i]);
        //     gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
        // }
        if (path != NULL) {
            // Sélectionner l'élément sous le curseur sans déclencher de clic
            gtk_tree_view_set_cursor(treeview, path, NULL, FALSE);
            gtk_tree_path_free(path);
            return TRUE;
        }
        menu = gtk_menu_new();
        for (int i = 0; i < num_options_V; i++) {
            GtkWidget *menu_item = gtk_menu_item_new_with_label(array_du_select_V[i]);
            // Créer et peupler la structure de données personnalisée
            MenuItemData *data   = g_new(MenuItemData, 1);
            data->index    = i;
            data->treeview = treeview;
            // Connecter le signal "activate" et passer la structure de données personnalisée en tant que user_data
            g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate_V), data);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
        }
        gtk_widget_show_all(menu);
        gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
        }
        // else if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_PRIMARY) {
        //             g_print("Clic simple détecté!\n");
        if (event->type == GDK_DOUBLE_BUTTON_PRESS) {  
            g_print("Double-clic détecté!\n");
            // g_signal_handler_disconnect(treeview, key_press_handler_id_launch);
            // g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_return, label2);
            open_file(treeview, user_data, current_appli);
            // key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            // key_press_handler_id_launch = g_signal_connect(treeview, "key-press-event", G_CALLBACK(open_file), combobox);
            generic_quit (treeview, pidFilename);
            return TRUE;
        }
    //     if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_PRIMARY) {  // Vérifiez si c'est un double-clic
    //             // g_print("Double-clic détecté!\n");
    //         g_signal_handler_disconnect(treeview, key_press_handler_id);
    //         g_signal_handler_disconnect(treeview, key_press_handler_id_launch);
    //         gint x, y;
    //         // Récupérer la position du clic
    //         GdkDevice *device = gdk_event_get_device((const GdkEvent *)event);
    //         GdkWindow *window = gtk_tree_view_get_bin_window(GTK_TREE_VIEW(treeview));

    //         gdk_window_get_device_position(window, device, &x, &y, NULL);

    //         // Récupérer le chemin de la ligne sous le curseur
    //         GtkTreePath *path = NULL;
    //         gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), x, y, &path, NULL, NULL, NULL);

    //         if (path != NULL) {
    //             // Sélectionner la ligne sous le curseur
    //             GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    //             gtk_tree_selection_select_path(selection, path);

    //             // Donner le focus à la ligne sous le curseur
    //             gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview), path, NULL, FALSE);

    //             // Libérer la mémoire du chemin
    //             gtk_tree_path_free(path);
    //         }
    //         // make_label_blink (neon_blink_timeout_id_return, label2);
    //         // open_file(treeview, user_data, current_appli);
    //         replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
    //         gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
    //         gtk_widget_grab_focus(GTK_WIDGET(treeview));
    //         key_press_handler_id_launch = g_signal_connect(treeview, "key-press-event", G_CALLBACK(open_file), combobox);
    //         key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
    //         // generic_quit (treeview, pidFilename);
    //         return TRUE;
    // }
    return FALSE;
}

gboolean on_treeview_key_press_left(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (widget != NULL) {
        switch (event->keyval) { 
            case GDK_KEY_Right: 
            case GDK_KEY_x: 
            case GDK_KEY_space: 
            case GDK_KEY_Escape:
                gtk_widget_hide(widget);
                element_neon_off(GTK_WIDGET(label7));
                break;
        }
    }
    return FALSE;
}

gboolean on_treeview_key_press_right(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (widget != NULL) {
        switch (event->keyval) { 
            case GDK_KEY_Left: 
            case GDK_KEY_x: 
            case GDK_KEY_space: 
            case GDK_KEY_Escape:
                gtk_widget_hide(widget);
                element_neon_off(GTK_WIDGET(label9));
                break;
        }
    }
    return FALSE;
}

gboolean on_treeview_key_press_bottom(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (widget != NULL) {
        switch (event->keyval) { 
            case GDK_KEY_Left: 
            case GDK_KEY_Right: 
            case GDK_KEY_x: 
            case GDK_KEY_space: 
            case GDK_KEY_Escape:
                gtk_widget_hide(widget);
                element_neon_off(GTK_WIDGET(label8));
                break;
        }
    }
    return FALSE;
}

void create_mini_menu(GtkWidget *menu, GtkWidget *widget,const int num_options, const gchar* array_du_select[], const gchar *sens) {
    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    g_signal_handler_disconnect(treeview, key_press_handler_id);
    menu = gtk_menu_new();
    if      (strcmp(sens, "right") == 0) g_signal_connect(menu, "key-press-event", G_CALLBACK(on_treeview_key_press_right), NULL);
    else if (strcmp(sens, "left")  == 0) g_signal_connect(menu, "key-press-event", G_CALLBACK(on_treeview_key_press_left) , NULL);
    else if (strcmp(sens, "bottom")  == 0) g_signal_connect(menu, "key-press-event", G_CALLBACK(on_treeview_key_press_bottom) , NULL);
    for (int i = 0; i < num_options; i++) {
        GtkWidget *menu_item = gtk_menu_item_new_with_label(array_du_select[i]);
        // Créer et peupler la structure de données personnalisée
        MenuItemData *data = g_new(MenuItemData, 1);
        data->index = i;
        data->treeview = treeview;
        // Connecter le signal "activate" et passer la structure de données personnalisée en tant que user_data
        if      (strcmp(sens, "right") == 0) g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate_R), data);
        else if (strcmp(sens, "left")  == 0) g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate_L), data);
        else if (strcmp(sens, "bottom")  == 0) g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate_B), data);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    }
    gtk_widget_show_all(menu);
    if      (strcmp(sens, "right") == 0) gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(treeview))), GDK_GRAVITY_EAST, GDK_GRAVITY_WEST, NULL);
    else if (strcmp(sens, "left")  == 0) gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(treeview))), GDK_GRAVITY_WEST, GDK_GRAVITY_EAST, NULL);
    else if (strcmp(sens, "bottom")  == 0) gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(treeview))), GDK_GRAVITY_SOUTH, GDK_GRAVITY_SOUTH, NULL);
    
    key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
}

gboolean on_treeview_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    GtkWidget *left_menu = NULL;
    GtkWidget *right_menu = NULL;
    GtkWidget *bottom_menu = NULL;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    switch (event->keyval) {
        case GDK_KEY_S:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openADS\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openads");
                current_appli = "openads";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
        case GDK_KEY_A:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openAria\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openaria");
                current_appli = "openaria";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_C:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openCimetiere\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opencimetiere");
                current_appli = "opencimetiere";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_D:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openDébitDeBoisson\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opendebitdeboisson");
                current_appli = "opendebitdeboisson";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_E:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openElec\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openelec");
                current_appli = "openelec";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_R:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openResultat\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openresultat");
                current_appli = "openresultat";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_r:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_n, label5);
            launch_terminal(treeview, "cat /home/sdethyre/sandbox/dash_dev_boisson/shortcuts.txt", "intern_minal", "");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE; 
            break;
        case GDK_KEY_n:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            launch_file(treeview, "/home/sdethyre/sandbox/auto_boisson/./dev_boisson", current_appli);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE; 
            break;
        case GDK_KEY_i:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_local_info(treeview, show_last_commit);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE; 
            break;
        case GDK_KEY_m:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            launch_terminal(treeview, "", "merge","");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE; 
            break;
        case GDK_KEY_c:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            show_commit_dialog(treeview, NULL, NULL);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_t:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            open_web_page(treeview, user_data, "open_ti.py" , current_appli, combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_l:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *full_text;
                gtk_tree_model_get(model, &iter, 1, &full_text, -1);
                // printf("%s", full_text);
                gchar *command = g_strdup_printf("google-chrome --new-tab \"/home/sdethyre/public_html/%s/%s/tests/results/log.html\"", current_appli, full_text);
                execute_command(command);
            }
            // open_web_page(treeview, user_data, "open_ti.py" , combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Alt_L:
            // element_neon_on(GTK_WIDGET(label6));
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:tests");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Devs");
            gtk_widget_grab_focus(GTK_WIDGET(button_tests));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Tab:
            // element_neon_on(GTK_WIDGET(label6));
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:tests");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Devs");
            gtk_widget_grab_focus(GTK_WIDGET(button_tests));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;

        case GDK_KEY_ISO_Level3_Shift:
            element_neon_on(GTK_WIDGET(label3));
            make_label_blink (neon_blink_timeout_id_alt_gr, label3);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            // remove_css_class_from_widget(combobox_appli, "neon-cimetiere-slow"); 
            // apply_css_class_on_widget(combobox_appli, "neon-cimetiere-fast");
            gtk_widget_grab_focus(GTK_WIDGET(combobox_appli));
            gtk_combo_box_popup(GTK_COMBO_BOX(combobox_appli));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_space:
            if (event->state & GDK_CONTROL_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // make_label_blink (neon_blink_timeout_id_space, label8);
                open_file(treeview, user_data, current_appli);
                open_web_page(treeview, user_data, "open_job.py", current_appli, combobox);
                open_web_page(treeview, user_data, "open_ti.py" , current_appli, combobox);
                open_web_page(treeview, user_data, "open_app.py", current_appli, combobox);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                generic_quit (treeview, pidFilename);
                return TRUE;
            }
            element_neon_on(GTK_WIDGET(label8));
            create_mini_menu(bottom_menu, widget, num_options_B, array_du_select_B, "bottom");
            return TRUE;
            break;
        case GDK_KEY_Return:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            // make_label_blink (neon_blink_timeout_id_return, label2);
            open_file(treeview, user_data, current_appli);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_Left:
            element_neon_on(GTK_WIDGET(label7));
            create_mini_menu(left_menu, widget, num_options_L, array_du_select_L, "left");
            return TRUE;
            break;
        case GDK_KEY_Right:
            element_neon_on(GTK_WIDGET(label9));
            create_mini_menu(right_menu, widget, num_options_R, array_du_select_R, "right");
            return TRUE;
            break;
        case GDK_KEY_Control_R:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_ctrl_D, label6);
            launch_test(treeview, current_appli);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_less:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_less, label1);
            open_web_page(treeview, user_data, "open_app_no_init.py", current_appli, combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_Escape:
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_x:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_w:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (popup_info != NULL) break;
            create_jenkins_info(treeview, show_last_commit);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_g:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (popup_graph != NULL) break;
            create_graph_fails(treeview);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_b:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (popup_builds != NULL) break;
            create_builds_list(treeview);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        default:
            break;
    }
    return FALSE;
}
gchar *full_texte;
// Signal "changed" de la sélection
static void on_selection_changed(GtkTreeSelection *selection, gpointer user_data) {
    // GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
    gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        shrink_intern_minal();
        
        gtk_tree_model_get(model, &iter, 1, &full_texte, -1);

        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        // Convertion du chemin en un indice d'itération (index) dans le modèle
        int index = gtk_tree_path_get_indices(path)[0];
        // Libération de mémoire du GtkTreePath
        gtk_tree_path_free(path);
        // Mise à jour de la variable globale de l'index
        current_index = index;
        save_current_dev(treeview);
    } 
    else current_index = -1; // Aucune ligne n'est sélectionnée, réinitialisation de l'index
}
gchar* ajouterUnderscores(const gchar* chaine) {
    // Vérifier si la chaîne est valide
    if (chaine == NULL) {
        return NULL;
    }

    // Calculer la longueur de la nouvelle chaîne avec les underscores
    size_t longueur_chaine = strlen(chaine);
    size_t longueur_nouvelle_chaine = longueur_chaine * 2; // Au cas où chaque caractère nécessite un underscore

    // Allouer de la mémoire pour la nouvelle chaîne
    gchar* nouvelle_chaine = (gchar*)malloc(longueur_nouvelle_chaine + 1); // +1 pour le caractère nul de fin de chaîne
    if (nouvelle_chaine == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Parcourir la chaîne d'origine et ajouter des underscores
    size_t index_nouvelle_chaine = 0;
    gboolean dernier_caractere_underscore = FALSE;  // Indicateur pour le dernier caractère ajouté

    for (size_t i = 0; i < longueur_chaine; i++) {
        if (chaine[i] == ' ') {
            // Si le caractère est un espace, ajouter un underscore seulement s'il n'est pas le dernier
            if (i < longueur_chaine - 1) {
                nouvelle_chaine[index_nouvelle_chaine++] = '_';
                dernier_caractere_underscore = TRUE;
            }
        } else {
            // Copier le caractère d'origine
            nouvelle_chaine[index_nouvelle_chaine++] = chaine[i];
            dernier_caractere_underscore = FALSE;
        }
    }

    // Ajouter le caractère nul de fin de chaîne s'il n'a pas déjà été ajouté par le dernier caractère
    if (!dernier_caractere_underscore) {
        nouvelle_chaine[index_nouvelle_chaine] = '\0';
    } else {
        // Si le dernier caractère était un underscore, décrémenter la longueur de la chaîne
        nouvelle_chaine[index_nouvelle_chaine - 1] = '\0';
    }

    return nouvelle_chaine;
}

gchar *escamote_line(const gchar *line, const gchar *text_changed) {
    // Copie de la ligne originale
    gchar *escamoted_line = g_strdup(line);
    // Position de la première occurrence de "text_changed" dans la ligne
    gchar *text_changed_pos = strstr(escamoted_line, text_changed);
    // Si l'occurrence est trouvée et si elle ne correspond pas exactement à "text_changed"
    if (text_changed_pos && !g_str_has_suffix(text_changed_pos, text_changed)) {
        // Calculer la longueur de la partie à modifier dans la ligne
        guint length = text_changed_pos - escamoted_line + strlen(text_changed)+1;
        gchar *spaces = g_strdup_printf("    📎%*s ", length + 2, " ");
        // Copier les espaces dans la partie à modifier de la ligne
        memcpy(escamoted_line, spaces, length);
        g_free(spaces);
    }
    return escamoted_line;
}

void launch(GtkWidget *widget, gpointer user_data) {
    GtkTreeView *treeview = GTK_TREE_VIEW(user_data);
    const char *pidFilename = "/home/sdethyre/sandbox/dash_dev_boisson/pid.txt";
    gboolean checked_vs     = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_vs)  );
    gboolean checked_ti     = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_ti)  );
    gboolean checked_job    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_job) );
    gboolean checked_app    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_app) );
    if      (checked_vs)      open_file(treeview, user_data, current_appli)                             ;
    if      (checked_ti)      open_web_page(treeview, user_data, "open_ti.py" , current_appli, combobox);
    if      (checked_job)     open_web_page(treeview, user_data, "open_job.py", current_appli, combobox);
    if      (checked_app)     open_web_page(treeview, user_data, "open_app.py", current_appli, combobox);
    generic_quit (treeview, pidFilename);
}

void gtk_main_quit_te() {
    const char *pidFilename = "/home/sdethyre/sandbox/dash_dev_boisson/pid.txt";
    clear_file(pidFilename);
    gtk_main_quit();
}

void handle_close_signal(int signum) {
    if (window != NULL) {
        gtk_widget_destroy(GTK_WIDGET(window));
        gtk_main_quit_te();
    }
}

int get_first_instance_PID() {
    int pid = -1;
    FILE *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/pid.txt", "r");
    if (file != NULL) {
        fscanf(file, "%d", &pid);
        fclose(file);
    }
    return pid;
}
void extract_selected_text(GtkTreeView *treeview, int column_index, gchar **result) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;

    // Vérifier si une ligne est sélectionnée
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *text;
        // Récupérer le texte de la colonne spécifiée par l'indice
        gtk_tree_model_get(model, &iter, column_index, &text, -1);

        // Allouer de la mémoire pour le résultat et copier le texte
        *result = strdup(text);

        // Libérer la mémoire allouée par GTK
        g_free(text);
    } else {
        // Si aucune ligne n'est sélectionnée, retourner une chaîne vide
        *result = strdup("");
    }
}
void update_selected_style(GtkTreeSelection *selection, gpointer user_data) {
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkStyleContext *context = GTK_STYLE_CONTEXT(user_data);
        gtk_style_context_add_class(context, "treeview-selected");
    } else {
        GtkStyleContext *context = GTK_STYLE_CONTEXT(user_data);
        gtk_style_context_remove_class(context, "treeview-selected");
    }
}
gchar *test_simple = NULL;
gchar *test_cache = NULL;
gchar *test_courrant = NULL;

gboolean is_visible_test = FALSE;
gboolean is_verbose_test = FALSE;
gboolean is_init_enabled = TRUE;

void get_current_test() {
    gchar *test_file;

    if (strcmp(current_appli, "openads") == 0) {
        test_file = "current_test_openads.txt";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        test_file = "current_test_openaria.txt";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        test_file = "current_test_opencimetiere.txt";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        test_file = "current_test_openboisson.txt";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        test_file = "current_test_openelec.txt";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        test_file = "current_test_openresultat.txt";
    }
    
    gchar *test_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/testFiles/%s", test_file);;
    test_cache = get_elements_at_right_of_string(test_path, "xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t");

    if (test_cache == NULL) {
        test_simple = get_elements_at_right_of_string(test_path, "om-tests -c runone -t");
    }

    // Check if test_simple is a valid string before manipulating it
    if (test_simple != NULL && strcmp(test_simple, "") != 0) {
        // Trim leading and trailing whitespaces
        test_simple = g_strstrip(test_simple);
        test_courrant = test_simple;  // Assign test_courrant after stripping whitespaces
        is_visible_test = TRUE;

        // Update is_init_enabled based on whether the suffix was removed
        if (g_str_has_suffix(test_simple, "--noinit")) {
            // The string ends with the specified suffix, adjust the length
            size_t len_str = strlen(test_simple);
            size_t len_suffix = strlen("--noinit");
            test_simple[len_str - len_suffix] = '\0';
            is_init_enabled = FALSE;
        } else {
            is_init_enabled = TRUE;
        }
    }

    // Check if test_cache is a valid string before manipulating it
    else if (test_cache != NULL && strcmp(test_cache, "") != 0) {
        // Trim leading and trailing whitespaces
        test_cache = g_strstrip(test_cache);
        test_courrant = test_cache;  // Assign test_courrant after stripping whitespaces
        is_visible_test = FALSE;

        // Update is_init_enabled based on whether the suffix was removed
        if (g_str_has_suffix(test_cache, "--noinit")) {
            // The string ends with the specified suffix, adjust the length
            size_t len_str = strlen(test_cache);
            size_t len_suffix = strlen("--noinit");
            test_cache[len_str - len_suffix] = '\0';
            is_init_enabled = FALSE; 
        } else {
            is_init_enabled = TRUE;
        }
    }

    // printf("Test courrant : %s\n", test_courrant);

    // Debugging information
    // printf("After removing suffix - Test simple: %s\n", test_simple);
    // printf("After removing suffix - Test caché: %s\n", test_cache);

    // Free allocated memory
    // g_free(test_simple);
    // g_free(test_cache);
}


// Fonction de callback pour le toggle "Avec affichage"/"Sans affichage"
void on_toggle_with_display_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean is_display_enabled = gtk_toggle_button_get_active(toggle_button);
    get_current_test();
    // Faire quelque chose avec la valeur, par exemple, activer ou désactiver l'affichage
    const gchar *new_text;
    if(is_display_enabled== FALSE) {
        gtk_button_set_image(GTK_BUTTON(toggle_button), NULL);
        GtkWidget *image = gtk_image_new_from_file("/home/sdethyre/sandbox/dash_dev_boisson/terminal.png");
        if (image == NULL) g_printerr("Error loading image\n");
        gtk_button_set_image(GTK_BUTTON(toggle_button), image);
         // Vérifier si les images des boutons sont activées (par défaut c'est le cas)
        gboolean button_images_enabled;
        g_object_get(gtk_settings_get_default(), "gtk-button-images", &button_images_enabled, NULL);
        if (!button_images_enabled) g_object_set(gtk_settings_get_default(), "gtk-button-images", TRUE, NULL);
        // new_text = "Sans affichage";
        new_text = "";
        gchar *full_texte_underscore;
        if (test_simple != NULL) full_texte_underscore= ajouterUnderscores(test_simple);
        else full_texte_underscore= ajouterUnderscores(test_cache);
        test_cache = NULL;
        test_simple=NULL;
        gchar *command;

        gchar *test_file;
        if (strcmp(current_appli, "openads") == 0) {
            test_file = "current_test_openads.txt";
        }
        if (strcmp(current_appli, "openaria") == 0) {
            test_file = "current_test_openaria.txt";
        }
        if (strcmp(current_appli, "opencimetiere") == 0) {
            test_file = "current_test_opencimetiere.txt";
        }
        if (strcmp(current_appli, "opendebitdeboisson") == 0) {
            test_file = "current_test_openboisson.txt";
        }
        if (strcmp(current_appli, "openelec") == 0) {
            test_file = "current_test_openelec.txt";
        }
        if (strcmp(current_appli, "openresultat") == 0) {
            test_file = "current_test_openresultat.txt";
        }
        gchar *test_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/testFiles/%s", test_file);

        if(is_init_enabled == TRUE) command = g_strdup_printf("echo \"xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t %s\" > %s", full_texte_underscore, test_path);
        else  command = g_strdup_printf("echo \"xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t %s --noinit\" > %s", full_texte_underscore, test_path);
        execute_command(command);
        is_visible_test = FALSE;
    }
    if(is_display_enabled== TRUE) {
        gtk_button_set_image(GTK_BUTTON(toggle_button), NULL);
        GtkWidget *image = gtk_image_new_from_file("/home/sdethyre/sandbox/dash_dev_boisson/appli.png");
        if (image == NULL) g_printerr("Error loading image\n");
        gtk_button_set_image(GTK_BUTTON(toggle_button), image);
         // Vérifier si les images des boutons sont activées (par défaut c'est le cas)
        gboolean button_images_enabled;
        g_object_get(gtk_settings_get_default(), "gtk-button-images", &button_images_enabled, NULL);
        if (!button_images_enabled) g_object_set(gtk_settings_get_default(), "gtk-button-images", TRUE, NULL);
        // gtk_button_set_label(GTK_BUTTON(toggle_with_display), "");

        // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_with_display), TRUE);
        gchar *full_texte_underscore;
        // new_text = "🎞 Avec affichage";
        new_text = "";
        if (test_simple != NULL) full_texte_underscore= ajouterUnderscores(test_simple);
        else full_texte_underscore= ajouterUnderscores(test_cache);
        test_cache = NULL;
        test_simple=NULL;
        gchar *command;

        gchar *test_file;
        if (strcmp(current_appli, "openads") == 0) {
            test_file = "current_test_openads.txt";
        }
        if (strcmp(current_appli, "openaria") == 0) {
            test_file = "current_test_openaria.txt";
        }
        if (strcmp(current_appli, "opencimetiere") == 0) {
            test_file = "current_test_opencimetiere.txt";
        }
        if (strcmp(current_appli, "opendebitdeboisson") == 0) {
            test_file = "current_test_openboisson.txt";
        }
        if (strcmp(current_appli, "openelec") == 0) {
            test_file = "current_test_openelec.txt";
        }
        if (strcmp(current_appli, "openresultat") == 0) {
            test_file = "current_test_openresultat.txt";
        }
        gchar *test_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/testFiles/%s", test_file);

        if(is_init_enabled == TRUE) command = g_strdup_printf("echo \"om-tests -c runone -t %s\" > %s", full_texte_underscore, test_path);
        else command = g_strdup_printf("echo \"om-tests -c runone -t %s --noinit\" > %s", full_texte_underscore, test_path);
        execute_command(command);
        is_visible_test = TRUE;
    }
    
    gtk_button_set_label(GTK_BUTTON(toggle_button), new_text);
}

void on_toggle_verbose(GtkToggleButton *toggle_button, gpointer user_data) {
    const gchar *new_text;
    if(is_verbose_test== TRUE) {
        new_text = "Silencieux";
        
        if(strcmp(current_appli, "opendebitdeboisson") == 0) replace_line("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "verbose_mode = True", "verbose_mode = False");
        else replace_line("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "verbose_mode = True", "verbose_mode = False");
        is_verbose_test = FALSE;
    }
    else if(is_verbose_test== FALSE) {
        new_text = "Verbeux";
        if(strcmp(current_appli, "opendebitdeboisson") == 0) replace_line("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "verbose_mode = False", "verbose_mode = True");
        else replace_line("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "verbose_mode = False", "verbose_mode = True");
        is_verbose_test = TRUE;
    }
    gtk_button_set_label(GTK_BUTTON(toggle_button), new_text);
}

// Fonction de callback pour le toggle "init"/"no init"
void on_toggle_init_toggled(GtkToggleButton *toggle_button, gpointer user_data) {
    gboolean is_init_enabled = gtk_toggle_button_get_active(toggle_button);
    get_current_test();
    const gchar *new_text;
    gchar *test_file;
    if (strcmp(current_appli, "openads") == 0) {
        test_file = "current_test_openads.txt";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        test_file = "current_test_openaria.txt";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        test_file = "current_test_opencimetiere.txt";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        test_file = "current_test_openboisson.txt";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        test_file = "current_test_openelec.txt";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        test_file = "current_test_openresultat.txt";
    }
    gchar *test_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/testFiles/%s", test_file);
    if(is_init_enabled== TRUE) {
        new_text = "No Init";
        gchar *full_texte_underscore;
        if (test_simple != NULL) full_texte_underscore= ajouterUnderscores(test_simple);
        else full_texte_underscore= ajouterUnderscores(test_cache);
        test_cache = NULL;
        gchar *command;

        if(is_visible_test == TRUE) command = g_strdup_printf("echo \"om-tests -c runone -t %s --noinit\" > %s", full_texte_underscore, test_path);
        else command = g_strdup_printf("echo \"xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t %s --noinit\" > %s", full_texte_underscore, test_path);
        execute_command(command);
        is_init_enabled = TRUE;
    }
    else if(is_init_enabled== FALSE) {
        new_text = "Init";
        gchar *full_texte_underscore;
        if (test_simple != NULL) full_texte_underscore= ajouterUnderscores(test_simple);
        else full_texte_underscore= ajouterUnderscores(test_cache);
        test_cache = NULL;
        gchar *command;
        if(is_visible_test == TRUE) command = g_strdup_printf("echo \"om-tests -c runone -t %s\" > %s", full_texte_underscore,test_path );
        else command = g_strdup_printf("echo \"xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t %s\" > %s", full_texte_underscore, test_path);
        execute_command(command);
        is_init_enabled = FALSE;
    }
    // Faire quelque chose avec la valeur, par exemple, gérer l'initialisation
    //  const gchar *new_text = is_init_enabled ? "No init" : "Init";
    gtk_button_set_label(GTK_BUTTON(toggle_button), new_text);
}

gchar *test_selected;
gchar *selected_value_with_underscores = NULL;
gboolean remove_suffix(gchar *str, const gchar *suffix) {
    if (g_str_has_suffix(str, suffix)) {
        // The string ends with the specified suffix, adjust the length
        size_t len_str = strlen(str);
        size_t len_suffix = strlen(suffix);
        str[len_str - len_suffix] = '\0';
        return TRUE;
    }
    return FALSE;
}

gchar *set_button_name_display(const gchar *original_button_name, gint characters_displayed) {
    // Allouez de la mémoire pour la nouvelle chaîne tronquée.
    gchar *button_name = g_malloc(characters_displayed + 5);  // 4 pour les trois points, 1 pour l'espace, et 1 pour le caractère nul.

    // Tronquez la chaîne et copiez-la dans le nouvel espace alloué.
    g_strlcpy(button_name, original_button_name, characters_displayed + 1);

    // Ajoutez l'espace et les trois points à la fin de la chaîne tronquée.
    g_strlcat(button_name, " ...", characters_displayed + 5);

    return button_name;
}

void on_row_activated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata) {
    get_current_test();
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value_with_underscores;

    // Obtenir la valeur avec des underscores de la colonne
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &value_with_underscores, -1);

        // Stocker la valeur avec des underscores dans la variable globale
        g_free(selected_value_with_underscores); // Libérer la mémoire précédente
        selected_value_with_underscores = g_strdup(value_with_underscores);
        if (g_utf8_strlen(value_with_underscores, -1) > 35) {

            gchar *final_name = set_button_name_display(value_with_underscores, 25);
            gtk_button_set_label(GTK_BUTTON(button_tests), final_name);
        }
        else gtk_button_set_label(GTK_BUTTON(button_tests), value_with_underscores);
        // gtk_button_set_label(GTK_BUTTON(button_tests), value_with_underscores);
        test_selected = value_with_underscores;
        gtk_widget_set_tooltip_text(button_tests, value_with_underscores);

        gchar *underscored = ajouterUnderscores(test_selected); 
        gchar *command;
        test_courrant = g_strdup_printf("%s.robot", underscored);
        gchar *test_file;
        if (strcmp(current_appli, "openads") == 0) {
            test_file = "current_test_openads.txt";
        }
        if (strcmp(current_appli, "openaria") == 0) {
            test_file = "current_test_openaria.txt";
        }
        if (strcmp(current_appli, "opencimetiere") == 0) {
            test_file = "current_test_opencimetiere.txt";
        }
        if (strcmp(current_appli, "opendebitdeboisson") == 0) {
            test_file = "current_test_openboisson.txt";
        }
        if (strcmp(current_appli, "openelec") == 0) {
            test_file = "current_test_openelec.txt";
        }
        if (strcmp(current_appli, "openresultat") == 0) {
            test_file = "current_test_openresultat.txt";
        }
        gchar *test_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/testFiles/%s", test_file);

        if(is_visible_test == TRUE && is_init_enabled == TRUE) command = g_strdup_printf("echo \"om-tests -c runone -t %s.robot\" > %s", underscored, test_path);
        if(is_visible_test == TRUE && is_init_enabled == FALSE) command = g_strdup_printf("echo \"om-tests -c runone -t %s.robot --noinit\" > %s", underscored, test_path);
        if(is_visible_test == FALSE && is_init_enabled == TRUE) command = g_strdup_printf("echo \"xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t %s.robot\" > %s", underscored, test_path);
        if(is_visible_test == FALSE && is_init_enabled == FALSE) command = g_strdup_printf("echo \"xvfb-run -a --server-args=-screen\\ 0\\ 1680x1050x24 om-tests -c runone -t %s.robot --noinit\" > %s", underscored, test_path);
        execute_command(command);
        // Obtenir la fenêtre parente et la détruire
        GtkWidget *top_level = gtk_widget_get_toplevel(GTK_WIDGET(treeview));
        gtk_widget_destroy(top_level);
    }
}

int compare_files(const void *a, const void *b) {
    return strcoll(*(const char**)a, *(const char**)b);
}
GtkWidget *window_tests;

static gboolean on_test_shown_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    switch (event->keyval) {
        case GDK_KEY_Escape:
            if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
            if (GTK_IS_WIDGET(window_tests)) gtk_widget_destroy(window_tests);
            return TRUE;
            break;
        case GDK_KEY_x:
            if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
            if (GTK_IS_WIDGET(window_tests)) gtk_widget_destroy(window_tests);
            return TRUE;
            break;
    }
    return FALSE;
}

void show_directory_content(GtkWidget *parent, const gchar *directory_path) {
    DIR *dir;
    struct dirent *ent;

    // Ouvrir le répertoire
    dir = opendir(directory_path);
    if (dir != NULL) {
        // Créer une nouvelle fenêtre
        window_tests = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window_tests), "Liste des tests");
        gtk_window_set_default_size(GTK_WINDOW(window_tests), 400, 700);
        g_signal_connect(G_OBJECT(window_tests), "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

        // Créer une boîte verticale (vbox) pour contenir les éléments
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_add(GTK_CONTAINER(window_tests), vbox);

        // Créer un modèle de liste
        GtkListStore *list_store = gtk_list_store_new(1, G_TYPE_STRING);

        // Créer une vue de liste pour afficher le contenu
        GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
        g_object_unref(list_store); // Le modèle de liste sera géré par le treeview

        // Ajouter une colonne à la vue de la liste
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Fichiers", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        // Créer une fenêtre de défilement pour la vue de la liste
        GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(scrolled_window), treeview);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

        // Ajouter la fenêtre de défilement à la boîte
        gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

        // Tableau pour stocker les noms de fichiers
        GPtrArray *file_names = g_ptr_array_new();

        // Parcourir les fichiers du répertoire et ajouter les noms au tableau
        while ((ent = readdir(dir)) != NULL) {
            gchar *file_name = ent->d_name;
            size_t length = strlen(file_name);

            if (length > 6 && strcmp(file_name + length - 6, ".robot") == 0) {
                g_ptr_array_add(file_names, g_strdup(file_name));
            }
        }

        // Fermer le répertoire
        closedir(dir);

        // Trier le tableau des noms de fichiers
        g_ptr_array_sort(file_names, (GCompareFunc)compare_files);

        // Ajouter les noms triés à la liste
        for (guint i = 0; i < file_names->len; i++) {
            gchar *file_name = g_ptr_array_index(file_names, i);

            // Ajouter le nom du fichier au modèle de liste
            GtkTreeIter iter;
            gtk_list_store_append(list_store, &iter);

            // Enlever les underscores et ".robot" du nom du fichier pour l'affichage dans la liste
            gchar *display_name = g_strdup(file_name);
            gchar *pos = display_name;
            while ((pos = strchr(pos, '_')) != NULL) {
                *pos = ' ';
            }
            display_name[strlen(file_name) - 6] = '\0'; // Supprimer le suffixe ".robot" de l'affichage

            gtk_list_store_set(list_store, &iter, 0, display_name, -1);

            // Libérer la mémoire
            g_free(display_name);
            g_free(file_name);
        }

        // Libérer la mémoire du tableau
        g_ptr_array_free(file_names, TRUE);

        // Connecter l'événement "row-activated" pour traiter la sélection d'une ligne
        g_signal_connect(G_OBJECT(treeview), "row-activated", G_CALLBACK(on_row_activated), NULL);
        g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_test_shown_key_press), NULL);
        // Afficher la fenêtre
        gtk_widget_show_all(window_tests);
    } else {
        g_print("Impossible d'ouvrir le répertoire\n");
    }
}

// Fonction de rappel pour le bouton "Tests"
void on_tests_button_clicked(GtkWidget *button, gpointer user_data) {
    replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:tests");
    gtk_label_set_text(GTK_LABEL(label5), "Alt G : Devs");
    const gchar *directory_path = g_strdup_printf("/home/sdethyre/public_html/%s/%s/tests", current_appli, full_texte);; // Remplacez par le chemin de votre répertoire
    // const gchar *directory_path = "/home/sdethyre/public_html/opendebitdeboisson/3.0.0-dev14/tests"; // Remplacez par le chemin de votre répertoire
    // printf("%s",full_texte);
    show_directory_content(GTK_WIDGET(user_data), directory_path);
}

void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    // Libérer la mémoire de la variable globale
    g_free(selected_value_with_underscores);
    gtk_main_quit();
}

static gboolean on_toggle_box_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GtkTreeView *treeview = GTK_TREE_VIEW(user_data);
    switch (event->keyval) {
        case GDK_KEY_S:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openADS\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openads");
                current_appli = "openads";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
        case GDK_KEY_A:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openAria\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openaria");
                current_appli = "openaria";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_C:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openCimetiere\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opencimetiere");
                current_appli = "opencimetiere";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_D:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openDébitDeBoisson\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opendebitdeboisson");
                current_appli = "opendebitdeboisson";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_E:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openElec\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openelec");
                current_appli = "openelec";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_R:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openResultat\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openresultat");
                current_appli = "openresultat";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_Control_R:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_ctrl_D, label6);
            launch_test(treeview, current_appli);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_l:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
            GtkTreeModel *model;
            GtkTreeIter iter;
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *full_text;
                gtk_tree_model_get(model, &iter, 1, &full_text, -1);
                // printf("%s", full_text);
                gchar *command = g_strdup_printf("google-chrome --new-tab \"/home/sdethyre/public_html/%s/%s/tests/results/log.html\"", current_appli, full_text);
                execute_command(command);
            }
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Alt_L:
            // element_neon_on(GTK_WIDGET(label5));
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(treeview));
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Tab:
            // element_neon_on(GTK_WIDGET(label5));
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(treeview));
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        // case GDK_KEY_Down:
        //     // element_neon_on(GTK_WIDGET(label5));
        //     // g_signal_handler_disconnect(treeview, key_press_handler_id);
        //     // gtk_widget_grab_focus(GTK_WIDGET(treeview));
        //     // key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
        //     return TRUE;
        //     break;
        case GDK_KEY_Up:
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
            // element_neon_on(GTK_WIDGET(label5));
            // g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(treeview));
            // key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_ISO_Level3_Shift:
            element_neon_on(GTK_WIDGET(label3));
            make_label_blink (neon_blink_timeout_id_alt_gr, label3);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(combobox_appli));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_less:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_less, label1);
            open_web_page(treeview, user_data, "open_app_no_init.py", current_appli, combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_x:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
    }
    return FALSE;
}

static gboolean on_slider_speed_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GtkTreeView *treeview = GTK_TREE_VIEW(user_data);
    switch (event->keyval) {
        case GDK_KEY_S:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openADS\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openads");
                current_appli = "openads";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
        case GDK_KEY_A:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openAria\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openaria");
                current_appli = "openaria";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_C:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openCimetiere\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opencimetiere");
                current_appli = "opencimetiere";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_D:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openDébitDeBoisson\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opendebitdeboisson");
                current_appli = "opendebitdeboisson";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_E:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openElec\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openelec");
                current_appli = "openelec";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_R:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openResultat\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openresultat");
                current_appli = "openresultat";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_Control_R:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_ctrl_D, label6);
            launch_test(treeview, current_appli);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_less:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_less, label1);
            open_web_page(treeview, user_data, "open_app_no_init.py", current_appli, combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_l:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
            GtkTreeModel *model;
            GtkTreeIter iter;
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *full_text;
                gtk_tree_model_get(model, &iter, 1, &full_text, -1);
                // printf("%s", full_text);
                gchar *command = g_strdup_printf("google-chrome --new-tab \"/home/sdethyre/public_html/%s/%s/tests/results/log.html\"", current_appli, full_text);
                execute_command(command);
            }
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Alt_L:
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(treeview));
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Tab:
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:tests");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Devs");
            gtk_widget_grab_focus(GTK_WIDGET(button_tests));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Up:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(button_tests));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Down:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(slider_skip));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Right:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            double currentValue = gtk_range_get_value(GTK_RANGE(slider_speed));

            // Définir la nouvelle valeur en ajoutant un incrément (par exemple, 0.1)
            double increment = 0.1;
            double newValue = currentValue + increment;

            // S'assurer que la nouvelle valeur est dans la plage du slider
            newValue = MIN(MAX(newValue, 0), 2);

            // Définir la nouvelle valeur du slider
            gtk_range_set_value(GTK_RANGE(slider_speed), newValue);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_ISO_Level3_Shift:
            element_neon_on(GTK_WIDGET(label3));
            make_label_blink (neon_blink_timeout_id_alt_gr, label3);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(combobox_appli));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_x:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
    }
    return FALSE;
}

static gboolean on_slider_skip_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    GtkTreeView *treeview = GTK_TREE_VIEW(user_data);
    switch (event->keyval) {
        case GDK_KEY_S:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openADS\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openads");
                current_appli = "openads";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
        case GDK_KEY_A:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openAria\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openaria");
                current_appli = "openaria";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_C:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openCimetiere\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opencimetiere");
                current_appli = "opencimetiere";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_D:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openDébitDeBoisson\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:opendebitdeboisson");
                current_appli = "opendebitdeboisson";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_E:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openElec\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openelec");
                current_appli = "openelec";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_R:
            if (event->state & GDK_SHIFT_MASK) {
                g_signal_handler_disconnect(treeview, key_press_handler_id);
                // g_print("Ligne sélectionnée : openResultat\n");
                replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", "current_appli:openresultat");
                current_appli = "openresultat";
                g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
                key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
                return TRUE; 
            }
            break;
        case GDK_KEY_Control_R:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_ctrl_D, label6);
            launch_test(treeview, current_appli);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_less:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_less, label1);
            open_web_page(treeview, user_data, "open_app_no_init.py", current_appli, combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
        case GDK_KEY_l:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
            GtkTreeModel *model;
            GtkTreeIter iter;
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *full_text;
                gtk_tree_model_get(model, &iter, 1, &full_text, -1);
                // printf("%s", full_text);
                gchar *command = g_strdup_printf("google-chrome --new-tab \"/home/sdethyre/public_html/%s/%s/tests/results/log.html\"", current_appli, full_text);
                execute_command(command);
            }
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Alt_L:
            // element_neon_on(GTK_WIDGET(label5));
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(treeview));
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:devs");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Tests");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_Tab:
            make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:tests");
            gtk_label_set_text(GTK_LABEL(label5), "Alt G : Devs");
            gtk_widget_grab_focus(GTK_WIDGET(button_tests));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        // case GDK_KEY_Down:
        //     // element_neon_on(GTK_WIDGET(label5));
        //     // g_signal_handler_disconnect(treeview, key_press_handler_id);
        //     // gtk_widget_grab_focus(GTK_WIDGET(treeview));
        //     // key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
        //     return TRUE;
        //     break;
        case GDK_KEY_Up:
            // make_label_blink (neon_blink_timeout_id_alt_gr, label5);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            // replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:", "current_focus:tests");
            // gtk_label_set_text(GTK_LABEL(label5), "Alt G : Devs");
            gtk_widget_grab_focus(GTK_WIDGET(slider_speed));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_ISO_Level3_Shift:
            element_neon_on(GTK_WIDGET(label3));
            make_label_blink (neon_blink_timeout_id_alt_gr, label3);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            gtk_widget_grab_focus(GTK_WIDGET(combobox_appli));
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            return TRUE;
            break;
        case GDK_KEY_x:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (GTK_IS_WIDGET(popup_info)) gtk_widget_destroy(popup_info);
            generic_quit (treeview, pidFilename);
            return TRUE;
            break;
    }
    return FALSE;
}


GList *test_cases_list(const char *filename, const char *startPattern) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char buffer[4096];
    bool shouldPrint = false;
    GList *test_cases_title = NULL;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (!shouldPrint && strstr(buffer, startPattern) != NULL) {
            shouldPrint = true;  // Démarre l'impression à partir de ce point
        }

        if (shouldPrint && buffer[0] != '\0' && !isspace((unsigned char)buffer[0])) {
            // Exclure les lignes commençant par "*** Test Cases"
            if (strncmp(buffer, "*** Test Cases", strlen("*** Test Cases")) != 0) {
                test_cases_title = g_list_append(test_cases_title, g_strdup(buffer));
            }
        }
    }

    fclose(file);
    return test_cases_title;
}


void ajouterAuDessusDeString(const char *nomFichier, const char *chaineRecherche, const char *nouvelleLigne) {
    FILE *fichier = fopen(nomFichier, "r");
    
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    char ligne[256];
    long positionTrouvee = -1;
    long positionPrecedente = -1;
    long positionCourante = ftell(fichier);

    // Recherche de la chaîne
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        positionCourante = ftell(fichier);
        if (strstr(ligne, chaineRecherche) != NULL) {
            positionTrouvee = positionPrecedente;
            break;
        }
        positionPrecedente = positionCourante;
    }

    fclose(fichier);

    if (positionTrouvee != -1) {
        // Ouvrir le fichier en mode lecture (r)
        fichier = fopen(nomFichier, "r");

        if (fichier == NULL) {
            perror("Erreur lors de l'ouverture du fichier");
            return;
        }

        // Créer un fichier temporaire
        FILE *tempFile = tmpfile();

        // Copier le contenu jusqu'à la position trouvée dans le fichier temporaire
        fseek(fichier, 0, SEEK_SET);
        while (ftell(fichier) < positionTrouvee) {
            if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
                break;
            }
            fprintf(tempFile, "%s", ligne);
        }

        // Écrire la nouvelle ligne dans le fichier temporaire
        fprintf(tempFile, "%s\n", nouvelleLigne);

        // Copier le reste du fichier dans le fichier temporaire
        while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
            fprintf(tempFile, "%s", ligne);
        }

        // Fermer les fichiers
        fclose(fichier);
        rewind(tempFile);

        // Ouvrir le fichier original en mode écriture (w)
        fichier = fopen(nomFichier, "w");
        if (fichier == NULL) {
            perror("Erreur lors de l'ouverture du fichier");
            fclose(tempFile);
            return;
        }

        // Copier le contenu du fichier temporaire dans le fichier original
        while (fgets(ligne, sizeof(ligne), tempFile) != NULL) {
            fprintf(fichier, "%s", ligne);
        }

        // Fermer les fichiers
        fclose(fichier);
        fclose(tempFile);
    } else {
        printf("Chaîne \"%s\" non trouvée dans le fichier.\n", chaineRecherche);
    }
}

gchar* trouverChaineSuivante(gchar *chaineRecherche, GPtrArray *liste) {
    for (int i = 0; i < liste->len; i++) {
        const gchar *chaineCourante = g_ptr_array_index(liste, i);
        if (g_strcmp0(chaineCourante, chaineRecherche) == 0) {
            // Trouvé la chaîne, vérifier si la chaîne suivante existe
            if (i + 1 < liste->len) {
                return g_ptr_array_index(liste, i + 1);
            } else {
                // La chaîne actuelle est la dernière dans le GPtrArray
                return NULL;
            }
        }
    }

    // La chaîne recherchée n'a pas été trouvée
    return NULL;
}

// Structure pour stocker les widgets
typedef struct {
    GtkWidget *treeview;
} WidgetsData;
GtkWidget *window_test_case;
// Fonction de rappel pour gérer le clic sur le bouton "Sélectionner"
void on_reset_button_clicked(GtkWidget *button, WidgetsData *data) {
    const gchar *directory_path = g_strdup_printf("/home/sdethyre/public_html/%s/%s/tests/%s", current_appli, full_texte, test_courrant);
    const gchar *command = g_strdup_printf("svn revert %s", directory_path);
    execute_command(command);
     gtk_widget_destroy(GTK_WIDGET(window_test_case));
}
// void on_cancel_button_clicked(GtkWidget *button, WidgetsData *data) {
//     const gchar *directory_path = g_strdup_printf("/home/sdethyre/public_html/opendebitdeboisson/%s/tests/%s", full_texte, test_courrant);
//     // const gchar *command = g_strdup_printf("svn revert %s", directory_path);
//     // execute_command(command);
//     FILE *file = fopen(directory_path, "r");
//     remplacerCaractereUniqueDebutLigne(file, '*', "");
//      gtk_widget_destroy(GTK_WIDGET(window_test_case));
// }
// Fonction de rappel pour gérer le clic sur le bouton "Sélectionner"
void on_select_button_clicked(GtkWidget *button, WidgetsData *data) {
    // Récupérer la sélection
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(data->treeview));

    // Vérifier la validité du modèle
    if (model == NULL || !GTK_IS_TREE_MODEL(model)) {
        g_print("Invalid tree model\n");
        // Handle the error appropriately
        return;
    }

    GList *selected_rows = gtk_tree_selection_get_selected_rows(selection, &model);

    const gchar *directory_path = g_strdup_printf("/home/sdethyre/public_html/%s/%s/tests/%s", current_appli, full_texte, test_courrant);

    GList *test_cases_content = test_cases_list(directory_path, "*** Test Cases ***");
    GPtrArray *test_cases_names = g_ptr_array_new();
    GList *iter;

    // Remplir un tableau avec les noms des cas de test
    for (iter = test_cases_content; iter != NULL; iter = g_list_next(iter)) {
        gchar *line = iter->data;
        g_ptr_array_add(test_cases_names, g_strdup(line));
    }

    // Parcourir et traiter les lignes sélectionnées
    GList *row;
    for (row = selected_rows; row != NULL; row = g_list_next(row)) {
        GtkTreePath *path = row->data;
    // printf("%s", gtk_tree_path_to_string(path));
        // Récupérer l'itérateur de la ligne
        GtkTreeIter iter;
        if (!gtk_tree_model_get_iter(model, &iter, path)) {
            g_print("Erreur lors de la récupération de l'itérateur pour le chemin %s\n", gtk_tree_path_to_string(path));
            continue;
        }

        // Vous pouvez récupérer les données de la ligne ici et les traiter selon vos besoins
        gchar *file_name;
        gtk_tree_model_get(model, &iter, 0, &file_name, -1);
        ajouterAuDessusDeString(directory_path, file_name, "*test case");
        g_print("Test case sélectionné : %s\n", file_name);

        const gchar *test_suivant = trouverChaineSuivante(file_name, test_cases_names);
        if(test_suivant !=NULL && g_strcmp0(test_suivant, "") != 0) ajouterAuDessusDeString(directory_path, test_suivant, "*");
        g_print("Test case suivant : %s\n", test_suivant);

        g_free(file_name);
        gtk_tree_path_free(path);
    }

    // Libérer la mémoire de la liste des chemins sélectionnés
    // g_list_free_full(selected_rows, (GDestroyNotify)gtk_tree_path_free);
    g_ptr_array_free(test_cases_names, TRUE);

    // Désactiver la fenêtre après le traitement
    gtk_widget_destroy(GTK_WIDGET(window_test_case));
}

void show_test_cases_list(GList *test_cases_content) {
    if (test_cases_content != NULL) {
        // Créer une nouvelle fenêtre
        window_test_case = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window_test_case), "Contenu du test");
        gtk_window_set_default_size(GTK_WINDOW(window_test_case), 300, 200);
        g_signal_connect(G_OBJECT(window_test_case), "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

        // Créer une boîte verticale (vbox) pour contenir les éléments
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_add(GTK_CONTAINER(window_test_case), vbox);

        // Créer un modèle de liste
        GtkListStore *list_store = gtk_list_store_new(1, G_TYPE_STRING);

        // Créer une vue de liste pour afficher le contenu
        GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
        g_object_unref(list_store); // Le modèle de liste sera géré par le treeview
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
        gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
        // Ajouter une colonne à la vue de la liste
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Fichiers", renderer, "text", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

        // Ajouter la vue de la liste à la boîte
        gtk_box_pack_start(GTK_BOX(vbox), treeview, TRUE, TRUE, 0);
        WidgetsData *data = g_new(WidgetsData, 1);
        data->treeview = treeview;

        // Créer un bouton "Reset"
        GtkWidget *reset_button = gtk_button_new_with_label("Reset");
        g_signal_connect(G_OBJECT(reset_button), "clicked", G_CALLBACK(on_reset_button_clicked), data);
        gtk_box_pack_end(GTK_BOX(vbox), reset_button, FALSE, FALSE, 0);

        // Créer un bouton "Sélectionner"
        GtkWidget *select_button = gtk_button_new_with_label("Sélectionner");
        g_signal_connect(G_OBJECT(select_button), "clicked", G_CALLBACK(on_select_button_clicked), data);
        gtk_box_pack_end(GTK_BOX(vbox), select_button, FALSE, FALSE, 0);

        // // Créer un bouton "Annuler"
        // GtkWidget *cancel_button = gtk_button_new_with_label("Annuler");
        // g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel_button_clicked), data);
        // gtk_box_pack_end(GTK_BOX(vbox), cancel_button, FALSE, FALSE, 0);

        // Tableau pour stocker les test cases
        GPtrArray *test_cases_names = g_ptr_array_new();
        GList *iter;
        for (iter = test_cases_content; iter != NULL; iter = g_list_next(iter)) {
            gchar *line = iter->data;
            // g_print("%s",line);
            g_ptr_array_add(test_cases_names, g_strdup(line));
        }

        // Trier le tableau des noms de fichiers
        // g_ptr_array_sort(test_cases_names, (GCompareFunc)compare_files);

        // Ajouter les noms triés à la liste
        for (guint i = 0; i < test_cases_names->len; i++) {
            gchar *file_name = g_ptr_array_index(test_cases_names, i);

            // Ajouter le nom du fichier au modèle de liste
            GtkTreeIter iter;
            gtk_list_store_append(list_store, &iter);

            // Enlever les underscores et ".robot" du nom du fichier pour l'affichage dans la liste
            gchar *display_name = g_strdup(file_name);
            gchar *pos = display_name;
            while ((pos = strchr(pos, '_')) != NULL) {
                // *pos = ' ';
            }
            // display_name[strlen(file_name) - 0] = '\0'; // Supprimer le suffixe ".robot" de l'affichage

            gtk_list_store_set(list_store, &iter, 0, display_name, -1);

            // Libérer la mémoire
            g_free(display_name);
            g_free(file_name);
        }

        // Libérer la mémoire du tableau
        g_ptr_array_free(test_cases_names, TRUE);

        // Connecter l'événement "row-activated" pour traiter la sélection d'une ligne
        g_signal_connect(G_OBJECT(treeview), "row-activated", G_CALLBACK(on_row_activated), NULL);

        // Afficher la fenêtre
        gtk_widget_show_all(window_test_case);
    } else {
        g_print("Impossible d'ouvrir le répertoire\n");
    }
}

void on_test_cases_button_clicked(GtkWidget *button, gpointer user_data) {
    const gchar *directory_path = g_strdup_printf("/home/sdethyre/public_html/%s/%s/tests/%s", current_appli, full_texte, test_courrant);
    GList *test_cases_content = test_cases_list(directory_path, "*** Test Cases ***");
    show_test_cases_list(test_cases_content);
}

GtkListStore *list_store;
void on_combobox_appli_changed(GtkComboBox *widget, gpointer user_data) {
    const gchar *selected_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
    if (selected_text != NULL) {
        // g_print("Ligne sélectionnée : %s\n", selected_text);
        gchar *index_temp = g_strdup_printf("current_appli:%s", selected_text);
        replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:", index_temp);
        current_appli = selected_text;
        g_free(index_temp);
        g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
        return;
    }
}
// Structure pour stocker les informations liées à l'animation
typedef struct {
    GtkWidget *drawing_area;
    guint animation_id;
      gdouble angle;
    gint rotation_direction;
    gint paused_rotation_direction;
} AppData;

// Fonction de dessin pour l'aire de dessin
gboolean draw_callback1(GtkWidget *widget, cairo_t *cr, gpointer data) {
    AppData *app_data = (AppData *)data;

    // Effacer l'aire de dessin
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);  // Couleur blanche
    cairo_paint(cr);
    // Dessiner quelque chose (par exemple, un rectangle tournant)
    cairo_translate(cr, 100, 100);
    cairo_rotate(cr, app_data->angle);
    cairo_rectangle(cr, -50, -50, 100, 100);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);  // Couleur noire
    cairo_fill(cr);

    return FALSE;
}
// Fonction d'animation

#include <gdk-pixbuf/gdk-pixbuf.h>  // Inclure la bibliothèque GDK Pixbuf


// Chemin de l'image BMP
#define BMP_IMAGE_PATH "/home/sdethyre/sandbox/dash_dev_boisson/332.png"

// Fonction de dessin pour l'aire de dessin
gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
    AppData *app_data = (AppData *)data;

    // Effacer l'aire de dessin
    cairo_set_source_rgb(cr, 0.1725, 0.1725, 0.1725);  // Couleur blanche
    cairo_paint(cr);

    // Charger l'image BMP avec GDK Pixbuf
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(BMP_IMAGE_PATH, NULL);

    if (pixbuf != NULL) {
        // Dessiner l'image tournante
        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);

        // Center the image within the drawing area
        double x_center = gtk_widget_get_allocated_width(widget) / 2.0;
        double y_center = gtk_widget_get_allocated_height(widget) / 2.0;

        cairo_translate(cr, x_center, y_center);
        cairo_rotate(cr, app_data->angle);
                // Dézoomer l'image en ajustant l'échelle
        double zoom_factor = 0.5;  // Ajustez le facteur de zoom selon vos besoins
        cairo_scale(cr, zoom_factor, zoom_factor);

        // Utiliser l'image comme source pour le dessin
        cairo_set_source_surface(cr, gdk_cairo_surface_create_from_pixbuf(pixbuf, 1.0, NULL), -width / 2.0, -height / 2.0);

        cairo_paint(cr);

        // Libérer la mémoire du GdkPixbuf
        g_object_unref(pixbuf); 
    }

    return FALSE;
}

gboolean update_animation(gpointer data) {
    AppData *app_data = (AppData *)data;

    // Mettre à jour l'angle pour la rotation en utilisant la direction de rotation
    app_data->angle += 0.1 * app_data->rotation_direction;

    // Redessiner l'aire de dessin
    gtk_widget_queue_draw(app_data->drawing_area);

    return G_SOURCE_CONTINUE;
}

void toggle_rotation_direction(GtkWidget *button, gpointer data) {
    AppData *app_data = (AppData *)data;

    // Ajouter un message de débogage
    // g_print("Toggle Rotation: Before - Rotation Direction: %d\n", app_data->rotation_direction);

    // Changer le sens de rotation
    app_data->rotation_direction = -app_data->rotation_direction;
    // app_data->rotation_direction = 2;

    // Ajouter un message de débogage
    // g_print("Toggle Rotation: After - Rotation Direction: %d\n", app_data->rotation_direction);

    // Redessiner l'aire de dessin pour refléter le nouveau sens de rotation
    gtk_widget_queue_draw(app_data->drawing_area);
}

gboolean rotation_paused = true;
gboolean rotation_first_use = true;
void pause_rotation_direction(GtkWidget *button, gpointer data) {
    AppData *app_data = (AppData *)data;

    // Ajouter un message de débogage
    // g_print("Toggle Rotation: Before - Rotation Direction: %d\n", app_data->rotation_direction);

    // Changer le sens de rotation
    // app_data->rotation_direction = -app_data->rotation_direction;
    if(rotation_first_use) {
        rotation_first_use = false;
        app_data->rotation_direction = 1;
    }
    else if(rotation_paused) {
        rotation_paused = false;
        app_data->paused_rotation_direction = app_data->rotation_direction;
        g_strdup_printf("Before %d",app_data->paused_rotation_direction );
        app_data->rotation_direction = 0;
        g_strdup_printf("After %d",app_data->paused_rotation_direction );
        
    }
    else if(!rotation_paused) {
        rotation_paused = true;
        app_data->rotation_direction = app_data->paused_rotation_direction;
    }
    // app_data->rotation_direction = 2;

    // Ajouter un message de débogage
    // g_print("Toggle Rotation: After - Rotation Direction: %d\n", app_data->rotation_direction);

    // Redessiner l'aire de dessin pour refléter le nouveau sens de rotation
    gtk_widget_queue_draw(app_data->drawing_area);
}
#include <math.h>
int compare_floats(float a, float b) {
    // Comparaison des nombres à virgule flottante
    if (fabs(a - b) <= FLT_EPSILON * fmaxf(1.0, fmaxf(fabs(a), fabs(b)))) {
        return 0;  // Les nombres sont considérés comme égaux
    } else if (a < b) {
        return -1; // a est inférieur à b
    } else {
        return 1;  // a est supérieur à b
    }
}


gboolean draw_toggle = false;
void toggle_visibility(GtkWidget *button, gpointer data) {
    AppData *app_data = (AppData *)data;

    if(draw_toggle) {
        draw_toggle = false;
        gtk_widget_set_size_request(app_data->drawing_area, -1, 0);
    }
    else {
            draw_toggle = true;
        gtk_widget_set_size_request(app_data->drawing_area, 200, 200);  // Ajustez la taille initiale selon vos besoins
    }
    // Inverser la visibilité du dessin
    // gboolean current_visibility = gtk_widget_get_visible(app_data->drawing_area);
    // gtk_widget_set_visible(app_data->drawing_area, !current_visibility);
}

static gchar *format_value_speed_callback(GtkScale *scale, gdouble value, gpointer user_data) {
    // Vous pouvez personnaliser la logique de formatage ici
    // Dans cet exemple, nous affichons le texte avec deux décimales
    if(compare_floats(value, 0.00) == 0) return g_strdup_printf("%s\n%.0f", "🐇", value);
    if(compare_floats(value, 0.20) == 0) return g_strdup_printf("%s\n%.1f", "🐇", value);
    if(compare_floats(value, 0.40) == 0) return g_strdup_printf("%s\n%.1f", "🦘", value);
    if(compare_floats(value, 0.60) == 0) return g_strdup_printf("%s\n%.1f", "🦘", value);
    if(compare_floats(value, 0.80) == 0) return g_strdup_printf("%s\n%.1f", "🦙", value);
    if(compare_floats(value, 1.00) == 0) return g_strdup_printf("%s\n%.1f", "🦙", value);
    if(compare_floats(value, 1.20) == 0) return g_strdup_printf("%s\n%.1f", "🐢", value);
    if(compare_floats(value, 1.40) == 0) return g_strdup_printf("%s\n%.1f", "🐢", value);
    if(compare_floats(value, 1.60) == 0) return g_strdup_printf("%s\n%.1f", "🦥", value);
    if(compare_floats(value, 1.80) == 0) return g_strdup_printf("%s\n%.1f", "🦥", value);
    if(compare_floats(value, 2.00) == 0) return g_strdup_printf("%s\n%.0f", "🦥", value);
    if(value > 0.00 && value < 0.40) {
        return g_strdup_printf("%s\n%.2f", "🐇", value);
    }
    if (value >= 0.40 && value < 0.80) {
        return g_strdup_printf("%s\n%.2f", "🦘", value);
    }
    if(value >= 0.80 && value < 1.20) {
        return g_strdup_printf("%s\n%.2f", "🦙", value);
    }
    if (value >= 1.20 && value <= 1.60) {
        return g_strdup_printf("%s\n%.2f", "🐢", value);
    }
    if(value > 1.60 && value < 2.00) {
        return g_strdup_printf("%s\n%.2f", "🦥", value);
    }
    return g_strdup_printf("%.2f", value);
}

static gchar *format_value_skip_callback(GtkScale *scale, gdouble value, gpointer user_data) {
    // Vous pouvez personnaliser la logique de formatage ici
    // Dans cet exemple, nous affichons le texte avec deux décimales
    // if(compare_floats(value, 0.00) == 0) return g_strdup_printf("%s\n%.0f", "🐇", value);
    // if(compare_floats(value, 0.20) == 0) return g_strdup_printf("%s\n%.1f", "🐇", value);
    // if(compare_floats(value, 0.40) == 0) return g_strdup_printf("%s\n%.1f", "🦘", value);
    // if(compare_floats(value, 0.60) == 0) return g_strdup_printf("%s\n%.1f", "🦘", value);
    // if(compare_floats(value, 0.80) == 0) return g_strdup_printf("%s\n%.1f", "🦙", value);
    // if(compare_floats(value, 1.00) == 0) return g_strdup_printf("%s\n%.1f", "🦙", value);
    // if(compare_floats(value, 1.20) == 0) return g_strdup_printf("%s\n%.1f", "🐢", value);
    // if(compare_floats(value, 1.40) == 0) return g_strdup_printf("%s\n%.1f", "🐢", value);
    // if(compare_floats(value, 1.60) == 0) return g_strdup_printf("%s\n%.1f", "🦥", value);
    // if(compare_floats(value, 1.80) == 0) return g_strdup_printf("%s\n%.1f", "🦥", value);
    // if(compare_floats(value, 2.00) == 0) return g_strdup_printf("%s\n%.0f", "🦥", value);
    // if(value > 0.00 && value < 0.40) {
    //     return g_strdup_printf("%s\n%.2f", "🐇", value);
    // }
    // if (value >= 0.40 && value < 0.80) {
    //     return g_strdup_printf("%s\n%.2f", "🦘", value);
    // }
    // if(value >= 0.80 && value < 1.20) {
    //     return g_strdup_printf("%s\n%.2f", "🦙", value);
    // }
    // if (value >= 1.20 && value <= 1.60) {
    //     return g_strdup_printf("%s\n%.2f", "🐢", value);
    // }
    // if(value > 1.60 && value < 2.00) {
    //     return g_strdup_printf("%s\n%.2f", "🦥", value);
    // }
    return g_strdup_printf("%.0f", value);
}
static void on_slider_speed_changed(GtkWidget *widget, gpointer data) {
    gdouble value = gtk_range_get_value(GTK_RANGE(widget));

    // Choisissez le nombre de décimales que vous souhaitez afficher
    int decimal_places = 2;
    
    gchar buffer[20];  // Choisissez une taille appropriée pour votre situation
    g_snprintf(buffer, sizeof(buffer), "%.*f", decimal_places, value);

    // Remplacez les virgules par des points dans la chaîne buffer
    for (int i = 0; buffer[i] != '\0'; ++i) {
        if (buffer[i] == ',') {
            buffer[i] = '.';
        }
    }

    gchar *formatted_speed = g_strdup_printf("chosen_speed = %s", buffer);
    if(strcmp(current_appli, "opendebitdeboisson") == 0) replace_line("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "chosen_speed =", formatted_speed);
    else replace_line("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "chosen_speed =", formatted_speed);
    
    // g_print("Nouvelle valeur du slider : %s\n", buffer);

    // Ne pas oublier de libérer la mémoire allouée
    g_free(formatted_speed);
}
static void on_slider_skip_changed(GtkWidget *widget, gpointer data) {
    gdouble value = gtk_range_get_value(GTK_RANGE(widget));

    // Choisissez le nombre de décimales que vous souhaitez afficher
    int decimal_places = 0;
    
    gchar buffer[20];  // Choisissez une taille appropriée pour votre situation
    g_snprintf(buffer, sizeof(buffer), "%.*f", decimal_places, value);

    // Remplacez les virgules par des points dans la chaîne buffer
    for (int i = 0; buffer[i] != '\0'; ++i) {
        if (buffer[i] == ',') {
            buffer[i] = '.';
        }
    }

    gchar *formatted_speed = g_strdup_printf("test_skip_number_hard = %s", buffer);

    if(strcmp(current_appli, "opendebitdeboisson") == 0) replace_line("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "test_skip_number_hard =", formatted_speed);
    else replace_line("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "test_skip_number_hard =", formatted_speed);
    // g_print("Nouvelle valeur du slider : %s\n", buffer);

    // Ne pas oublier de libérer la mémoire allouée
    g_free(formatted_speed);
}

int main(int argc, char *argv[]) {
    current_appli = g_strstrip(recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_appli:"));
    gchar *current_focus = g_strstrip(recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_focus:"));
    gchar *current_verbose_test;
    if(strcmp(current_appli, "opendebitdeboisson") == 0) current_verbose_test = g_strstrip(recuperer_elements_droite_string("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "verbose_mode = "));
    else current_verbose_test = g_strstrip(recuperer_elements_droite_string("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "verbose_mode = "));
    is_verbose_test = strcmp(g_strstrip(current_verbose_test), "False") == 0 ? FALSE : TRUE;
    // printf("%d", is_verbose_test);
    get_current_test();
    // Dimensions
    window_width = 600;
    number_of_devs = count_lines_in_file("/home/sdethyre/sandbox/dash_dev_boisson/results.txt");
    number_of_applis = count_lines_in_file("/home/sdethyre/sandbox/dash_dev_boisson/results_applis.txt");
    basic_height = 100 + 18 * number_of_devs;
    intern_minal_height = 300;

    if (is_instance_running()) {
        // Envoyer le signal SIGUSR1 à la première instance
        int first_instance_pid = get_first_instance_PID();
        if (first_instance_pid != -1) {
            kill(first_instance_pid, SIGUSR1);
            g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev_boisson", NULL);
            return EXIT_SUCCESS;
        }
    }

    int current_pid = getpid(); // Obtenir le PID de l'application en cours d'exécution
    save_PID_to_file(current_pid);

    char command[256];
    gchar *temp = g_strdup_printf("bash /home/sdethyre/sandbox/dash_dev_boisson/list_devs %s", current_appli);

    // Utilisez snprintf au lieu de sprintf
    snprintf(command, sizeof(command), "%s", temp);

    int exit_status = system(command);

    if (exit_status == -1) {printf("Erreur lors de l'exécution du script"); return 1;}
    else if (!WIFEXITED(exit_status) || WEXITSTATUS(exit_status) != 0) {printf("Le script s'est terminé avec une erreur");return 1;}

    char command_appli[256];
    sprintf(command_appli, "bash /home/sdethyre/sandbox/dash_dev_boisson/list_applis");
    int exit_status_appli = system(command_appli);

    if (exit_status_appli == -1) {printf("Erreur lors de l'exécution du script"); return 1;}
    else if (!WIFEXITED(exit_status_appli) || WEXITSTATUS(exit_status_appli) != 0) {printf("Le script s'est terminé avec une erreur");return 1;}

    gtk_init(&argc, &argv);
// Connecter la fonction de rappel au signal configure-event
    // Créer la fenêtre principale
    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(GTK_WINDOW(window), "Devs    🔧");
    gtk_window_set_default_size(GTK_WINDOW(window), window_width, basic_height);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    // g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(on_window_resize), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit_te), NULL);
    gtk_window_move(GTK_WINDOW(window), 750, 300);
        // Définir la transparence du fond de la fenêtre
    GdkDisplay *display = gdk_display_get_default();

    GdkScreen *screen   = gdk_display_get_default_screen(display);

    // GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(window), visual);
    }
        // Utilisation de CSS pour rendre la fenêtre transparente
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window {\n"
        "  background-color: rgba(0, 0, 0, 0.8); color: white;\n"
        "}\n",
        -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(window));
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    // Créer le modèle de données pour la liste
    list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

    // Ouvrir le fichier "results.txt"
    char file_path[256];
    sprintf(file_path, "/home/sdethyre/sandbox/dash_dev_boisson/results.txt");
    FILE *file = fopen(file_path, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            // Suppression du saut de ligne à la fin de la ligne
            line[strcspn(line, "\n")] = '\0';
            // Vérifier que la ligne ne se termine pas par ".bak"
            gboolean is_valid_line = !g_str_has_suffix(line, ".bak");
            
            if ((g_str_has_prefix(line, "3.0.0-develop")) && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "develop");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            else if ((g_str_has_prefix(line, "2.3.0-develop")) && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "develop");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            else if ((g_str_has_prefix(line, "4.2.0-b9_develop")) && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "develop");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            else if ((g_str_has_prefix(line, "4.3.2-develop")) && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "develop");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            else if ((g_str_has_prefix(line, "4.11.0-develop")) && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "develop");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            else if (g_str_has_prefix(line, "refonte-graphique") && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "");
                gchar *title = "refonte-graphique";
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter, 0, title, 1, escamoted_line, -1);
                g_free(escamoted_line);
            }
            else if (g_str_has_prefix(line, "rg-") && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "rg");
                gchar *indented_line = g_strdup_printf("    🎨    %s", escamoted_line);
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter, 0, indented_line, 1, line, -1);
                g_free(escamoted_line);
                g_free(indented_line);
            }
            else if (g_str_has_prefix(line, "evo-") && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "");
                gchar *indented_line = g_strdup_printf("    🎨    %s", escamoted_line);
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter, 0, indented_line, 1, line, -1);
                g_free(escamoted_line);
                g_free(indented_line);
            }
            else if (g_str_has_prefix(line, "fix-") && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "");
                gchar *indented_line = g_strdup_printf("    🎨    %s", escamoted_line);
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter, 0, indented_line, 1, line, -1);
                g_free(escamoted_line);
                g_free(indented_line);
            }
            if (( g_str_has_prefix(line, "6") || g_str_has_prefix(line, "v6") || g_str_has_prefix(line, "1") ) && is_valid_line) {
                gchar *escamoted_line = escamote_line(line, "develop");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "trunk") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "5") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "vdm0027520") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "10488") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "10322") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "support") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if (g_str_has_prefix(line, "10313") && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
            if ((g_str_has_prefix(line, "BUG") || g_str_has_prefix(line, "EVO") || g_str_has_prefix(line, "POC")) && is_valid_line) {
                // printf("%s",line);
                gchar *escamoted_line = escamote_line(line, "");
                GtkTreeIter iter;
                // Ajouter une nouvelle ligne à la liste GtkTreeStore
                gtk_list_store_append(list_store, &iter);
                // Définir les valeurs des colonnes de la nouvelle ligne
                gtk_list_store_set(list_store, &iter, 0, escamoted_line, 1, line, -1);
                g_free(escamoted_line);
            }
        }
        fclose(file);
    }

    // Une ligne vide de + dans la liste
    GtkTreeIter blank_iter;
    gtk_list_store_append(list_store, &blank_iter);
    // Définir les valeurs des colonnes de la nouvelle ligne (en laissant les valeurs vides)
    gtk_list_store_set(list_store, &blank_iter, 0, "", 1, "", -1);

    // Créer le GtkTreeView à partir du modèle de données
    treeview = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store)));
    g_object_unref(list_store);
    GtkCssProvider *treeview_provider = gtk_css_provider_new();
    gchar * rgb_value;

    if(strcmp(current_appli, "opendebitdeboisson") == 0) {
        rgb_value = "86, 207, 170";
    }

    if(strcmp(current_appli, "openads") == 0) {
        rgb_value = "89, 192, 237";
    }
    if(strcmp(current_appli, "openelec") == 0) {
        rgb_value = "97, 154, 243";
    }

    if(strcmp(current_appli, "openresultat") == 0) {
        rgb_value = "97, 154, 243";
    }
    if(strcmp(current_appli, "openaria") == 0) {
        rgb_value = "84, 137, 198";
    }

    if(strcmp(current_appli, "opencimetiere") == 0) {
        rgb_value = "171, 144, 243";
    }

    gchar * css_treeview = g_strdup_printf("treeview {\n"
        "  background-color: rgba(%s, 0.3);color:white;\n"
        "}\n", rgb_value);

    gtk_css_provider_load_from_data(treeview_provider, css_treeview, -1, NULL);

    // Obtenir le contexte de style du GtkTreeView
    GtkStyleContext *treeview_context = gtk_widget_get_style_context(GTK_WIDGET(treeview));

    // Ajouter le fournisseur CSS au contexte de style du GtkTreeView
    gtk_style_context_add_provider(treeview_context,
        GTK_STYLE_PROVIDER(treeview_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Libérer le fournisseur CSS (il est maintenant détenu par le contexte de style)
    g_object_unref(treeview_provider);

    // Créer et charger le fournisseur CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "treeview:selected {text-shadow: 0 0 2px #fff, 0 0 4px #fff, 0 0 6px #fff, 0 0 8px #308280, 0 0 20px #308280, 0 0 16px #308280, 0 0 20px #308280; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    //▶ ◀
    // Créer une colonne pour afficher le texte
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    // GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("\n  < : Appli\t\t\t      ⏎ : VsCode\t\t\t\t Ctrl D : Test  \n  Alt Gr : Choix test\t\t\t     n : Nouveau\t\t\t\t     x : Quitter\n  ⇦ Debug\t\t\t\t\t━━ : Tout\t\t\t\t\t   SVN ⇨ \n", renderer, "text", 0, NULL);
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Définir la taille du GtkTreeView
    GtkRequisition requisition;
    gtk_widget_get_preferred_size(GTK_WIDGET(treeview), &requisition, NULL);
    requisition.width = 200; // Modifier la largeur souhaitée
    gtk_widget_set_size_request(GTK_WIDGET(treeview), requisition.width, requisition.height);

    // Créer la sélection pour le GtkTreeView
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

    gchar *image_name;
    gchar *neon_class_name;
    gchar *current_dev_appli_entry;

    if (strcmp(current_appli, "openads") == 0) {
        current_dev_appli_entry = "current_dev_ads:";
        image_name = "openAds";
        neon_class_name = "neon-ads";
    }
    if (strcmp(current_appli, "openaria") == 0) {
        current_dev_appli_entry = "current_dev_aria:";
        image_name = "openAria";
        neon_class_name = "neon-aria";
    }
    if (strcmp(current_appli, "opencimetiere") == 0) {
        current_dev_appli_entry = "current_dev_cimetierre:";
        image_name = "openCimetiere";
        neon_class_name = "neon-cimetiere";
    }
    if (strcmp(current_appli, "opendebitdeboisson") == 0) {
        current_dev_appli_entry = "current_dev_debit:";
        image_name = "openDebit";
        neon_class_name = "neon-boisson";
    }
    if (strcmp(current_appli, "openelec") == 0) {
        current_dev_appli_entry = "current_dev_elec:";
        image_name = "openElec";
        neon_class_name = "neon-elec";
    }
    if (strcmp(current_appli, "openresultat") == 0) {
        current_dev_appli_entry = "current_dev_resultat:";
        image_name = "openResultat";
        neon_class_name = "neon-resultat";
    }
    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", current_dev_appli_entry);
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));


    // Connecter l'événement de sélection au callback "open_file"
    // key_press_handler_id_launch = g_signal_connect(G_OBJECT(treeview), "row-activated", G_CALLBACK(open_file), combobox);

    // Connecter l'événement de pression des touches du clavier au callback "on_treeview_key_press"
    g_signal_connect(G_OBJECT(treeview), "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);

    // Créer le conteneur vertical (vbox)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // ⇦ ⇨ ⟶ ⮕ ↵ ⤶ 
    // 3 * 3
    label1 = gtk_label_new("< : Appli");
    label2 = gtk_label_new("⏎ : VsCode");
    label3 = gtk_label_new("Alt Gr : Applis");
    label4 = gtk_label_new("x : Quitter");
    // gchar *label5_tmp;
    // if (strcmp(current_focus, "tests") == 0) label5_tmp = "Tests";
    // else label5_tmp = "Devs";
    gchar *label5_tmp = strcmp(current_focus, "tests") == 0 ? "Devs" : "Tests";
    gchar *label5_text = g_strdup_printf("Alt G : %s", label5_tmp);
    label5 = gtk_label_new(label5_text);
    label6 = gtk_label_new("Ctrl D  :   Test");
    label7 = gtk_label_new("⬅ Ticketing");
    // label8 = gtk_label_new("━━ : Tout");
    label8 = gtk_label_new("━━ : Outils");
    label9 = gtk_label_new("Versioning ➡");

    gtk_label_set_xalign (GTK_LABEL(label1), -1);
    gtk_label_set_xalign (GTK_LABEL(label4), -1);
    gtk_label_set_xalign (GTK_LABEL(label7), -1);
    gtk_label_set_xalign (GTK_LABEL(label3), 1);
    gtk_label_set_xalign (GTK_LABEL(label6), 1);
    gtk_label_set_xalign (GTK_LABEL(label9), 1);

    // Conteneur de grille 3x3
    GtkWidget *labels_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(labels_grid), TRUE); // Répartir l'espace également entre les colonnes

    gtk_grid_attach(GTK_GRID(labels_grid), label1, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label2, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label3, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label4, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label5, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label6, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label7, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label8, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(labels_grid), label9, 2, 2, 1, 1);

    gtk_widget_set_margin_bottom(labels_grid, 7);

    // Ajout de la grille au début du vbox
    gtk_box_pack_start(GTK_BOX(vbox), labels_grid, FALSE, FALSE, 10); // Ajuster les marges

    // GtkComboBox avec valeurs prédéfinies et champ d'entrée
    const gchar* combo_values[] = {"admin", "instr", "instrpoly", "admingen", "agmingenmars", "guichet", "guichetsuivi", "assist", "oaamb"};
    const int num_versions = sizeof(combo_values) / sizeof(combo_values[0]);
    combobox = gtk_combo_box_text_new_with_entry();
    GtkWidget *entry = gtk_bin_get_child(GTK_BIN(combobox));
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "admin");
    for (int i = 0; i < num_versions; i++) { gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), combo_values[i]); }
    gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combobox), 0);
    gtk_widget_set_tooltip_text(combobox, "Utilisateur de l'application (admin par défaut)");

    gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(treeview));
    // GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    // gtk_box_set_homogeneous(GTK_BOX(hbox), TRUE);
    button_tests = gtk_button_new_with_label("Tests");
    gchar *display_name = "Tests";
    if(test_simple!=NULL){
        display_name = g_strdup(test_simple);
        gchar *pos = display_name;
        while ((pos = strchr(pos, '_')) != NULL) {
            *pos = ' ';
        }
        if(is_init_enabled==TRUE) display_name[strlen(test_simple) - 6] = '\0'; // Supprimer le suffixe ".robot" de l'affichage
        else display_name[strlen(test_simple) - 7] = '\0';
        test_simple=NULL;
    }
    if(test_cache!=NULL){
        display_name = g_strdup(test_cache);
        gchar *pos = display_name;
        while ((pos = strchr(pos, '_')) != NULL) {
            *pos = ' ';
        }
        if(is_init_enabled==TRUE) display_name[strlen(test_cache) - 6] = '\0'; // Supprimer le suffixe ".robot" de l'affichage
        else display_name[strlen(test_cache) - 7] = '\0';
        test_cache = NULL;
        
    }
    if (g_utf8_strlen(display_name, -1) > 35) {
        gchar *final_name = set_button_name_display(display_name, 25);
        gtk_button_set_label(GTK_BUTTON(button_tests), final_name);
    }
    else gtk_button_set_label(GTK_BUTTON(button_tests), display_name);

    gtk_widget_set_tooltip_text(button_tests, display_name);

    GtkWidget *toggle_with_display = gtk_toggle_button_new_with_label("Sans affichage");
    // Définir le bouton comme déjà activé (toggled)
    if(is_visible_test == TRUE) {
        // Supprimer image résiduelle
        gtk_button_set_image(GTK_BUTTON(toggle_with_display), NULL);
        GtkWidget *image = gtk_image_new_from_file("/home/sdethyre/sandbox/dash_dev_boisson/appli.png");
        if (image == NULL) g_printerr("Error loading image\n");
        gtk_button_set_image(GTK_BUTTON(toggle_with_display), image);
         // Vérifier si les images des boutons sont activées (par défaut c'est le cas)
        gboolean button_images_enabled;
        g_object_get(gtk_settings_get_default(), "gtk-button-images", &button_images_enabled, NULL);
        if (!button_images_enabled) g_object_set(gtk_settings_get_default(), "gtk-button-images", TRUE, NULL);
        gtk_button_set_label(GTK_BUTTON(toggle_with_display), "");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_with_display), TRUE);
    }
    else {
        gtk_button_set_image(GTK_BUTTON(toggle_with_display), NULL);
        GtkWidget *image = gtk_image_new_from_file("/home/sdethyre/sandbox/dash_dev_boisson/terminal.png");
        if (image == NULL) g_printerr("Error loading image\n");
        gtk_button_set_image(GTK_BUTTON(toggle_with_display), image);
         // Vérifier si les images des boutons sont activées (par défaut c'est le cas)
        gboolean button_images_enabled;
        g_object_get(gtk_settings_get_default(), "gtk-button-images", &button_images_enabled, NULL);
        if (!button_images_enabled) g_object_set(gtk_settings_get_default(), "gtk-button-images", TRUE, NULL);
        gtk_button_set_label(GTK_BUTTON(toggle_with_display), "");
    }


    GtkWidget *toggle_verbose = gtk_toggle_button_new_with_label("Silencieux");
    if(is_verbose_test == TRUE) {
        gtk_button_set_label(GTK_BUTTON(toggle_verbose), "Verbeux");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_verbose), TRUE);
    }
    else gtk_button_set_label(GTK_BUTTON(toggle_verbose), "Silencieux");
    // Création du slider
    slider_speed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 2, 0.10);
    gtk_scale_set_digits(GTK_SCALE(slider_speed), 2); // Pas de chiffres décimaux
    gtk_scale_set_value_pos(GTK_SCALE(slider_speed), GTK_POS_TOP); // Position de la valeur en haut
    for (int i = 0; i < 21; ++i) { // Réduisez le nombre de marques
        double mark_value = i * 0.1; // Espacement plus important entre les marques
        gtk_scale_add_mark(GTK_SCALE(slider_speed), mark_value, GTK_POS_TOP, "");
    }
    gchar *vitesse_initiale;
        if(strcmp(current_appli, "opendebitdeboisson") == 0) vitesse_initiale = recuperer_elements_droite_string("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "chosen_speed = ");
        else vitesse_initiale = recuperer_elements_droite_string("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "chosen_speed = ");
    
    // printf("%s",vitesse_initiale);
    const gchar *str = "3.14";
    gchar *endptr;
    
    // Convertir la chaîne en double
    gdouble result = g_strtod(vitesse_initiale, &endptr);

    // Vérifier si la conversion a réussi
    if (result == 0.0 && str == endptr) {
        g_print("La conversion a échoué.\n");
    } else {
        // g_print("La valeur en double est : %f\n", result);
        gtk_range_set_value(GTK_RANGE(slider_speed), result);
    }

    g_signal_connect(slider_speed, "value-changed", G_CALLBACK(on_slider_speed_changed), NULL);
    g_signal_connect(G_OBJECT(slider_speed), "format-value", G_CALLBACK(format_value_speed_callback), NULL);

    slider_skip = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 15, 1);
    gtk_scale_set_digits(GTK_SCALE(slider_skip), 0); // Pas de chiffres décimaux
    gtk_scale_set_value_pos(GTK_SCALE(slider_skip), GTK_POS_TOP); // Position de la valeur en haut
    for (int i = 0; i < 16; ++i) {
        double mark_value = i * 1;
        gtk_scale_add_mark(GTK_SCALE(slider_skip), mark_value, GTK_POS_TOP, "");
    }
    gchar *tests_2_skip ;
    if(strcmp(current_appli, "opendebitdeboisson") == 0) tests_2_skip = recuperer_elements_droite_string("/home/sdethyre/.venv/p3-omdt/lib/python3.8/site-packages/openmairie/devtools/TestListener.py", "test_skip_number = ");
    else tests_2_skip = recuperer_elements_droite_string("/home/sdethyre/.venv/omdt/lib/python3.9/site-packages/openmairie/devtools/TestListener.py", "test_skip_number = "); 
    
    // printf("%s",vitesse_initiale);
    const gchar *str_2 = "3.14";
    gchar *endptr_2;
    
    // Convertir la chaîne en double
    gdouble result_2 = g_strtod(tests_2_skip, &endptr_2);

    // Vérifier si la conversion a réussi
    if (result_2 == 0.0 && str_2 == endptr_2) {
        g_print("La conversion a échoué.\n");
    } else {
        // g_print("La valeur en double est : %f\n", result_2);
        gtk_range_set_value(GTK_RANGE(slider_skip), result_2);
    }

    g_signal_connect(slider_skip, "value-changed", G_CALLBACK(on_slider_skip_changed), NULL);
    g_signal_connect(G_OBJECT(slider_skip), "format-value", G_CALLBACK(format_value_skip_callback), NULL);

    GtkWidget *toggle_init = gtk_toggle_button_new_with_label("Init");
    if(is_init_enabled == FALSE) {
        gtk_button_set_label(GTK_BUTTON(toggle_init), "No Init");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_init), TRUE);
    }
    else gtk_button_set_label(GTK_BUTTON(toggle_init), "Init");
    // Créer une boîte horizontale pour les widgets à droite du bouton "Tests"
    // Créer une boîte horizontale pour les widgets à droite du bouton "Tests"
    button_test_cases = gtk_button_new_with_label("Cases");
    
    GtkWidget *toggle_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    gtk_box_pack_start(GTK_BOX(toggle_box), button_tests, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(toggle_box), toggle_with_display, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(toggle_box), toggle_verbose, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(toggle_box), toggle_init, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(toggle_box), button_test_cases, TRUE, FALSE, 0);

    // Créer la boîte horizontale principale (hbox)
    GtkWidget *vbox_tests = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox_tests), TRUE);

    // Ajouter la boîte de basculement à la hbox
    gtk_box_pack_start(GTK_BOX(vbox_tests), toggle_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_tests), slider_speed, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_tests), slider_skip, FALSE, FALSE, 0);

    // Créer le conteneur de centrage horizontal
    GtkWidget *centering_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(centering_box), vbox_tests, TRUE, TRUE, 0);

    // Ajouter la boîte centrée à la vbox
    gtk_box_pack_start(GTK_BOX(vbox), centering_box, FALSE, FALSE, 10);

    g_signal_connect(G_OBJECT(toggle_box), "key-press-event", G_CALLBACK(on_toggle_box_key_press), treeview);

    g_signal_connect(G_OBJECT(slider_speed), "key-press-event", G_CALLBACK(on_slider_speed_key_press), treeview);
    g_signal_connect(G_OBJECT(slider_skip), "key-press-event", G_CALLBACK(on_slider_skip_key_press), treeview);

    // ...

    // // Espace visuel entre la grille et le bouton
    // GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    // gtk_widget_set_hexpand(separator, TRUE);
    // gtk_container_add(GTK_CONTAINER(vbox), separator);
    // gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(combobox));

    // ...

    // Connecter l'événement "clicked" des toggles à des callbacks appropriés
    g_signal_connect(G_OBJECT(button_tests), "clicked", G_CALLBACK(on_tests_button_clicked), window);
        // g_signal_connect(button_tests, "clicked", G_CALLBACK(launch), treeview);
    g_signal_connect(toggle_with_display, "toggled", G_CALLBACK(on_toggle_with_display_toggled), NULL);
    g_signal_connect(toggle_verbose, "toggled", G_CALLBACK(on_toggle_verbose), NULL);
    g_signal_connect(toggle_init, "toggled", G_CALLBACK(on_toggle_init_toggled), NULL);
    g_signal_connect(G_OBJECT(button_test_cases), "clicked", G_CALLBACK(on_test_cases_button_clicked), window);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_main_window_destroy), NULL);

    // Espace visuel entre la grille et le bouton
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(separator, TRUE);
    gtk_container_add(GTK_CONTAINER(vbox), separator);
    gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(combobox));

    // Conteneur GtkBox pour centrer le grid
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), box, TRUE, TRUE, 0);

    // Grille
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_margin_top(grid, 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(box), grid);

    // 2 lignes, 2 colonnes :
    // check_vs  = gtk_check_button_new_with_label("VsCode");
    // gtk_grid_attach(GTK_GRID(grid), check_vs,  0, 2, 1, 1);
    // check_ti  = gtk_check_button_new_with_label("Ticket Interne");
    // gtk_grid_attach(GTK_GRID(grid), check_ti,  1, 2, 1, 1);
    // check_job = gtk_check_button_new_with_label("Job Jenkins");
    // gtk_grid_attach(GTK_GRID(grid), check_job, 0, 3, 1, 1);
    // check_app = gtk_check_button_new_with_label("Application");
    // gtk_grid_attach(GTK_GRID(grid), check_app, 1, 3, 1, 1);

    // Sur une seule ligne
    check_vs  = gtk_check_button_new_with_label("VsCode");
    gtk_grid_attach(GTK_GRID(grid), check_vs,  0, 0, 1, 1);
    check_ti  = gtk_check_button_new_with_label("Ticket Interne");
    gtk_grid_attach(GTK_GRID(grid), check_ti,  1, 0, 1, 1);
    check_job = gtk_check_button_new_with_label("Job Jenkins");
    gtk_grid_attach(GTK_GRID(grid), check_job, 2, 0, 1, 1);
    check_app = gtk_check_button_new_with_label("Application");
    gtk_grid_attach(GTK_GRID(grid), check_app, 3, 0, 1, 1);

    // The "Modifiés" label and the new "Init" check button in a separate row with a slight right offset
    // counter_label = gtk_label_new("w : infos");
    // gtk_grid_attach(GTK_GRID(grid), counter_label, 0, 1, 1, 1);
    // gtk_widget_set_margin_start(counter_label, 130); // Adjust the value as needed

    // check_init = gtk_check_button_new_with_label("Init");
    // gtk_grid_attach(GTK_GRID(grid), check_init, 1, 4, 1, 1);
    // gtk_widget_set_margin_start(check_init, 30);

    // After connecting "row-activated" signal for treeview selection
    // GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(on_selection_changed), NULL);
    key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
    g_signal_connect(G_OBJECT(treeview), "button-press-event", G_CALLBACK(on_right_click), NULL);


/////////////////////////////////////////////////////////////////
    // GtkWidget *box_a = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    // gtk_container_add(GTK_CONTAINER(box), box_a);

    // AppData app_data = {
    //     .drawing_area = NULL,
    //     .animation_id = 0,
    //     .angle = 0.0,
    //     .rotation_direction = 0,  // Initialiser la direction de rotation à 1 (sens horaire)
    // };
    // // AppData app_data;
    // app_data.drawing_area = gtk_drawing_area_new();
    // gtk_widget_set_size_request(app_data.drawing_area, -1, -1);
    // gtk_box_pack_start(GTK_BOX(box), app_data.drawing_area, FALSE, FALSE, 0);  // Ajustez ces paramètres selon vos besoins
    // g_signal_connect(G_OBJECT(app_data.drawing_area), "draw", G_CALLBACK(draw_callback), &app_data);



    // GtkWidget *toggle_button_pause = gtk_button_new_with_label("Pause");
    // g_signal_connect(G_OBJECT(toggle_button_pause), "clicked", G_CALLBACK(pause_rotation_direction), &app_data);

    // // Ajouter le bouton à la boîte ou à une autre disposition de votre interface
    // gtk_box_pack_start(GTK_BOX(box), toggle_button_pause, FALSE, FALSE, 0);


    // GtkWidget *toggle_button = gtk_button_new_with_label("Changer Rotation");
    // g_signal_connect(G_OBJECT(toggle_button), "clicked", G_CALLBACK(toggle_rotation_direction), &app_data);

    // // Ajouter le bouton à la boîte ou à une autre disposition de votre interface
    // gtk_box_pack_start(GTK_BOX(box), toggle_button, FALSE, FALSE, 0);


    //  GtkWidget *toggle_visibility_button = gtk_button_new_with_label("Toggle Visibility");
    // g_signal_connect(G_OBJECT(toggle_visibility_button), "clicked", G_CALLBACK(toggle_visibility), &app_data);

    // // Ajouter le bouton à la boîte ou à une autre disposition de votre interface
    // gtk_box_pack_start(GTK_BOX(box), toggle_visibility_button, FALSE, FALSE, 0);


    // app_data.animation_id = g_timeout_add(16, update_animation, &app_data);

/////////////////////////////////////////////////////////////////

    GtkWidget *button = gtk_button_new_with_label("Ouvrir");
    gtk_container_add(GTK_CONTAINER(vbox), button);
    g_signal_connect(button, "clicked", G_CALLBACK(launch), treeview);


    gchar *image_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/%s_rounded.png", image_name);
    GdkPixbuf *pixbuf       = gdk_pixbuf_new_from_file_at_scale(image_path, 200, 100, TRUE, NULL);
    GtkWidget *logo_resized = gtk_image_new_from_pixbuf(pixbuf);
    
    apply_css_class_on_widget(logo_resized, neon_class_name);
    apply_css_class_on_widget(logo_resized, "border-radius");
    // gtk_widget_set_margin_top(logo_resized, 20);

    const gchar* combo_appli_values[number_of_applis];
    char file_path_appli[256];
        int index = 0;
    sprintf(file_path_appli, "/home/sdethyre/sandbox/dash_dev_boisson/results_applis.txt");
    FILE *file_appli = fopen(file_path_appli, "r");
    if (file_appli) {
        char line[256];
        while (fgets(line, sizeof(line), file_appli) && index < number_of_applis) {
            // printf("%s",line);
            // Supprimer le saut de ligne à la fin de la ligne si présent
            size_t len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }

            // Allouer de la mémoire pour la nouvelle chaîne et la copier
            if(line != NULL) combo_appli_values[index] = strdup(line);
            // Vérifier si l'allocation de mémoire a réussi
            if (combo_appli_values[index] == NULL) {
                fprintf(stderr, "Erreur d'allocation de mémoire\n");
                exit(EXIT_FAILURE);
            }

            index++;
        }
        fclose(file_appli);
    }
    else {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    const int num_versions_appli = sizeof(combo_appli_values) / sizeof(combo_appli_values[0]);
    // combobox_appli = gtk_combo_box_text_new_with_entry();
    combobox_appli = gtk_combo_box_text_new();

    // GtkWidget *entry_appli = gtk_bin_get_child(GTK_BIN(combobox_appli));
    // gtk_entry_set_placeholder_text(GTK_ENTRY(entry_appli), current_appli);

    for (int i = 0; i < num_versions_appli; i++) { gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox_appli), combo_appli_values[i]); }
    // Libérer la mémoire allouée pour chaque chaîne
    for (int i = 0; i < index; i++) {
        free((void *)combo_appli_values[i]);
    }
    // for (int i = 0; i < num_versions_appli; i++) { printf("zzzzzzzzzzzzzzzzzzz%s", combo_appli_values[i]); }
    // gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combobox_appli), 0);
    gtk_widget_set_tooltip_text(combobox_appli, "Application");

    // gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(combobox_appli));


    GtkWidget *hbox_appli = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(hbox_appli), TRUE);
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(hbox_appli), fixed);
    // Ajouter la boîte de basculement à la hbox
    // gtk_box_pack_start(GTK_BOX(hbox_appli), logo_resized, FALSE, FALSE, 0);
    int window_width;
    gtk_window_get_size(GTK_WINDOW(window), &window_width, NULL);
    printf("%d      ",window_width);
    // int x_position = (window_width - gtk_widget_get_allocated_width(logo_resized) - 205) / 2;
    int x_position = (window_width - 225) / 2;
    gtk_fixed_put(GTK_FIXED(fixed), combobox_appli, x_position, 5);
    gtk_fixed_put(GTK_FIXED(fixed), logo_resized, x_position, 5);
    gtk_widget_set_size_request(combobox_appli, 200, 48);
    // gtk_box_pack_start(GTK_BOX(hbox_appli), combobox_appli, FALSE, FALSE, 0);
    // Créer le conteneur de centrage horizontal
    GtkWidget *centering_box_appli = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(centering_box_appli), TRUE);
    gtk_box_pack_start(GTK_BOX(centering_box_appli), hbox_appli, TRUE, TRUE, 0);

    // Ajouter la boîte centrée à la vbox
    gtk_box_pack_start(GTK_BOX(vbox), centering_box_appli, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(combobox_appli), "changed", G_CALLBACK(on_combobox_appli_changed), NULL);

    //▼˅⌄
    terminal_button = GTK_BUTTON(gtk_button_new_with_label("▼"));
    gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(terminal_button));

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    g_signal_connect(terminal_button, "clicked", G_CALLBACK(toggle_intern_minal_height), scrolled_window);
    g_signal_connect(terminal_button, "clicked", G_CALLBACK(toggle_window_height_at_bottom), window);
    signal(SIGUSR1, handle_close_signal);

    // GtkTextView for displaying the output
    output_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(output_text_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), output_text_view);

    // GtkBox to hold the GtkScrolledWindow and the GtkButton
    GtkWidget *box_w = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box_w), scrolled_window, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(vbox), box_w);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_widget_hide(scrolled_window);

    save_current_dev(GTK_TREE_VIEW(treeview));
       gchar *selected_text;
    extract_selected_text(GTK_TREE_VIEW(treeview), 1, &selected_text);
    full_texte = selected_text;
    if (strcmp(current_focus, "tests") == 0) {
        gtk_widget_grab_focus(GTK_WIDGET(button_tests));
    }
    else gtk_widget_grab_focus(GTK_WIDGET(treeview));
    gtk_main();
    unlink(LOCK_FILE);
    return 0;
}
