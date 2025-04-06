#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h> 
#include <sys/wait.h> 
#include <glib.h>
#include <gdk/gdk.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include "include/fileman.h"
#include "include/launching.h"

#define LOCK_FILE "/tmp/my_program.lock"

int        current_index = -1;
gint       window_width;
gint       number_of_devs;
gint       basic_height; 
gint       intern_minal_height; 
guint      neon_blink_timeout_id_less   = 0;
guint      neon_blink_timeout_id_return = 0;
guint      neon_blink_timeout_id_ctrl_D = 0;
guint      neon_blink_timeout_id_alt_gr = 0;
guint      neon_blink_timeout_id_space  = 0;
guint      neon_blink_timeout_id_n      = 0;
 
gulong     key_press_handler_id;
gulong     key_press_handler_id_B;
gdouble    percent_by_time;
gdouble    percent_by_lines;

GtkWidget *check_vs, *check_ti, *check_job, *check_app, *check_init, *commit_window, *checkout_window,*menu = NULL, *combobox, *counter_label;
GtkWidget *label1, *label2, *label3, *label4, *label5, *label6, *label7, *label8, *label9;
GtkWidget *output_text_view = NULL;
GtkWindow *window;
// GtkWidget *window;
GtkWidget *popup = NULL;
GtkWidget *scrolled_window;
GtkButton *terminal_button;
GtkWidget *drawing_area;
GtkWidget *vbox;
gboolean   neon_blink_state_ctrl = FALSE;
gboolean   terminal_open = FALSE;

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
const gchar* array_du_select_L[] = { "📋     Logs", "📈     Jenkins", "📊     Infos", "📉     Fails", "📰     Output", "🔬     Report", "🔍     Results", "📺     Apache ", "🔄     Appli Init", "🆕     Nouveau", "📑     Ticket"};
const int    num_options_L = sizeof(array_du_select_L) / sizeof(array_du_select_L[0]);

const gchar* array_du_select_R[] = { "🌀     Meld", "📝     Commit", "📩     Up", "🔀     Merge", "🗑     Delete", "🔩     Modifiés", "📟     Statut", "👥     Diff", "📒     SVN Log", "📝     D.commit", "🔗     Checkout" };
const int    num_options_R = sizeof(array_du_select_R) / sizeof(array_du_select_R[0]);

const gchar* array_du_select_V[] = { "📈     Jenkins", "📊     Infos", "🌀     Meld", "📑     Ticket", "📋     Logs", "🆕     Nouveau", "━━━━━━━━━━", "📩     Up", "📝     Commit", "🔀     Merge", "🔬     Modifiés", "📟     Statut", "👥     Diff" , "🗑     Delete"};
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

void save_current_dev(GtkTreeView *treeview) {
    gchar *index_temp = g_strdup_printf("current_dev:%d", current_index);
    replace_line("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_dev:", index_temp);
}

void element_neon_on(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "/home/sdethyre/sandbox/dash_dev_boisson/style.css", NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_class(context, "neon");
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

void launch_terminal(GtkTreeView *treeview, gchar *command, gchar *how_to_close) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if(strcmp(how_to_close, "merge") == 0) {
        GError *error = NULL;
        gchar *command_line[] = { "/bin/bash", "-c", "/home/sdethyre/sandbox/merge/./merge", NULL };
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
            gchar *directory = g_strdup_printf("/home/sdethyre/public_html/openads/%s", branch_selected);

            if (chdir(directory) != 0) {
                g_print("Impossible de changer de répertoire.\n");
                g_free(directory);
                return;
            }
            gchar *final_command;
            if(strcmp(how_to_close, "intern_minal") == 0) {
                grow_intern_minal();
                execute_bash_intern(output_text_view, command); 
                return;
            } 
            if(strcmp(how_to_close, "last_commit") == 0) {
                grow_intern_minal();
                gchar *command_tmp = g_strdup_printf("svn log -r HEAD:1 --limit 1 svn+ssh://sdethyre@scm.adullact.net/svn/openfoncier/branches/%s", branch_selected);
                execute_bash_intern(output_text_view, command_tmp); 
                return;
            } 
            if     (strcmp(how_to_close, "return_key") == 0) final_command = g_strdup_printf("gnome-terminal -- bash -c '%s; read -s -r -p \"Appuyez sur Entrée pour fermer le terminal...\"'"   , command);
            else if(strcmp(how_to_close,    "any_key") == 0) final_command = g_strdup_printf("gnome-terminal -- bash -c '%s; read -n 1 -p \"Appuyez sur une touche pour fermer le terminal...\"'", command);
            else                                          final_command = g_strdup_printf("gnome-terminal -- bash -c '%s'", command);
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
            gchar *local_path = g_strdup_printf("/home/sdethyre/public_html/openads/%s", branch_selected);
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
                            if (revision_start  != NULL && propagated_end != NULL) {
                                // Extract the part of the line containing the revision number
                                char *revision_number_start = revision_start + strlen(revision_str);
                                size_t revision_number_length = propagated_end - revision_number_start;
                                char *revision_number = (char *)malloc(revision_number_length + 1);
                                strncpy(revision_number, revision_number_start, revision_number_length);
                                revision_number[revision_number_length] = '\0';

                                int revision = atoi(revision_number);
                                printf("Numéro de révision : %d\n", revision);
                                to_copy = g_strdup_printf("-r%d : %s", revision, commit_message);

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
        gchar *svn_command = g_strdup_printf("gnome-terminal -- bash -c 'cd /home/sdethyre/public_html/openads && svn checkout svn+ssh://sdethyre@scm.adullact.net/svn/openfoncier/branches/%s'", escaped_checkout_message);
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

        gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Delete");
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if (result != GTK_RESPONSE_YES) return;  // User chose not to delete the branch, return without doing anything
        
        gchar *directory = g_strdup_printf("/home/sdethyre/public_html/openads");
        if (chdir(directory) != 0) {
            g_print("Impossible de changer de répertoire.\n");
            g_free(directory);
            return;
        }
        gchar *svn_command = g_strdup_printf("svn delete svn+ssh://sdethyre@scm.adullact.net/svn/openfoncier/branches/%s -m \"chore(branch): suppression de la branche de développement %s\" > /home/sdethyre/sandbox/dash_dev_boisson/svn_output.txt", branch_selected, branch_selected);

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
                            to_copy = g_strdup_printf("-r%d : %s", revision, delete_message);

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

static void on_popup_close(GtkWidget *widget, gpointer user_data) { gtk_widget_destroy(popup); popup = NULL; }
static void on_popup_destroy(GtkWidget *widget, gpointer user_data) { popup = NULL; }

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
    gchar *command = g_strdup_printf("python3 %s %s", script_name, branch_name);
    FILE *fp = popen(command, "r");
    if (fp == NULL) g_free(command);
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) g_string_append(output, buffer);
    pclose(fp);
    g_free(command);
    g_strstrip(output->str);
    g_strchomp(output->str);
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
        else if (strcmp(output->str, "Dernier job incomplet") == 0) {
            output_label_tmp   = g_strdup("Dernier job ");
            output_label_left  = gtk_label_new(output_label_tmp);
            output_label_right = gtk_label_new("incomplet");
            element_neon_custom_on(GTK_WIDGET(output_label_right), "red");
        }
        else if (strcmp(output->str, "Job en cours") == 0) {
            output_label_tmp   = g_strdup("Job ");
            output_label_left  = gtk_label_new(output_label_tmp);
            output_label_right = gtk_label_new("en cours");
            element_neon_custom_on(GTK_WIDGET(output_label_right), "orange");
        }
        else {
            output_label_left  = gtk_label_new("Nombre de fails : ");
            output_label_tmp   = g_strdup(output->str);
            output_label_right = gtk_label_new(output_label_tmp);
            element_neon_custom_on(GTK_WIDGET(output_label_right), "red");
        }
        g_free(output_label_tmp);
    }
    else if (strcmp(script_name, "/home/sdethyre/sandbox/dash_dev_boisson/last_test_d.py") == 0 ) {
        // printf("out : %s",output->str);
        if (strcmp(context, "Dernier job incomplet") == 0) {
            // printf("%s",output->str);
            gchar *output_label_tmp   = g_strdup("Dernier test : ");
            output_label_left  = gtk_label_new(output_label_tmp);
            gchar *output_label_tmp_2 = g_strdup_printf("%s",output->str);
            output_label_right = gtk_label_new(output_label_tmp_2);
            element_neon_custom_on(GTK_WIDGET(output_label_right), "orange");
            g_free(output_label_tmp);
            g_free(output_label_tmp_2);
        }
        else if (strcmp(context, "Job en cours") == 0) {
            // printf("%s",output->str);
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

// void *async_create_line_popup(void *params) {
//     CreatePopupParams *popup_params = (CreatePopupParams *)params;
//     create_line_popup(popup_params->popup_box, popup_params->script_name, popup_params->branch_name);
//     g_free(popup_params->script_name);
//     g_free(popup_params->branch_name);
//     g_free(popup_params);
//     return NULL;
// }

void create_popup_info(GtkTreeView *treeview) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *branch_selected;

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
        gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);

    if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
        popup = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gchar *titre = g_strdup_printf("Infos sur %s", branch_selected);
        gtk_window_set_title(GTK_WINDOW(popup), titre);
        gtk_window_set_position(GTK_WINDOW(popup), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(popup), 400, 200);

        GtkWidget *popup_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(popup), popup_box);

        gchar directory[256] = "/home/sdethyre/public_html/openads/";
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

        gchar *command_populate_last_build = g_strdup_printf("python3 /home/sdethyre/sandbox/dash_dev_boisson/get_last_build_api_results.py %s", branch_selected);
        system(command_populate_last_build);
        g_free(command_populate_last_build);

        int status_last_build;
        wait(&status_last_build);
        
        gchar *command_populate_output = g_strdup_printf("python3 /home/sdethyre/sandbox/dash_dev_boisson/output_jenkins_to_file.py %s", branch_selected);
        system(command_populate_output);
        g_free(command_populate_output);

        int status_output;
        wait(&status_output);
        
        gchar *command = g_strdup_printf("python3 /home/sdethyre/sandbox/dash_dev_boisson/fails.py %s", branch_selected);
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
            create_line_popup(popup_box, "Job inexistant : erreur 404"                           , branch_selected, "404");
        }
        else if (strcmp(output->str, "Dernier job incomplet") == 0) {
            GtkWidget *inter_line = gtk_label_new("━━━━━━━━━━");  
            gtk_box_pack_start(GTK_BOX(popup_box), inter_line, TRUE, TRUE, 0);
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/fails_d.py"              , branch_selected, "Dernier job incomplet");                
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/date_hour_d.py"          , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/build_creator_d.py"      , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/last_test_d.py"          , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/count_display_fails_d.py", branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_d.py"            , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_by_line_d.py"    , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/moyenne.py"              , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/authors_d.py"            , branch_selected, "Dernier job incomplet");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/changes_jenkins_d.py"    , branch_selected, "Dernier job incomplet");
        }
        else if (strcmp(output->str, "Job en cours") == 0) {
            GtkWidget *inter_line = gtk_label_new("━━━━━━━━━━");  
            gtk_box_pack_start(GTK_BOX(popup_box), inter_line, TRUE, TRUE, 0);
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/fails_d.py"              , branch_selected, "Job en cours");                
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/date_hour_d.py"          , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/build_creator_d.py"      , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/last_test_d.py"          , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/count_display_fails_d.py", branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_d.py"            , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/percent_by_line_d.py"    , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/moyenne.py"              , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/authors_d.py"            , branch_selected, "Job en cours");
            create_line_popup(popup_box, "/home/sdethyre/sandbox/dash_dev_boisson/changes_jenkins_d.py"    , branch_selected, "Job en cours");
        }
        else {
            GtkWidget *inter_line = gtk_label_new("━━━━━━━━━━");  
            gtk_box_pack_start(GTK_BOX(popup_box), inter_line, TRUE, TRUE, 0);
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
        g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(on_popup_close), NULL);
        gtk_container_add(GTK_CONTAINER(button_box), close_button);

        g_signal_connect(G_OBJECT(popup), "destroy", G_CALLBACK(on_popup_destroy), NULL);
        gtk_widget_show_all(popup);
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
            launch_file(treeview, "/home/sdethyre/bin/atreal/./testLogs");
            break;
        case 1:
            open_web_page(treeview, user_data, "open_job.py", combobox);
            break;
        case 2:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_popup_info(treeview);
            break;
        case 3:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *branch_selected;
                gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
                if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
                    gchar *fails = g_strdup_printf("python3 /home/sdethyre/sandbox/dash_dev_boisson/display_fails.py %s", branch_selected);
                    launch_terminal(treeview, fails, "intern_minal");
                }
            }
            break;
        case 4:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gchar *branch_selected;
                gtk_tree_model_get(model, &iter, 1, &branch_selected, -1);
                if (branch_selected && !g_str_has_suffix(branch_selected, ".bak")) {
                    gchar *fails = g_strdup_printf("python3 /home/sdethyre/sandbox/dash_dev_boisson/output_jenkins.py %s", branch_selected);
                    launch_terminal(treeview, fails, "intern_minal");
                }
            }
            break;
        case 5:
            open_web_page(treeview, user_data, "open_job_report.py" , combobox);
            break;
        case 6:
            open_web_page(treeview, user_data, "open_job_results.py" , combobox);
            break;
        case 7:
            launch_terminal(treeview, "tail -f /var/log/apache2/error.log", "return_key");
            break;
        case 8:
            open_web_page(treeview, user_data, "open_app.py", combobox);
            break;
        case 9:
            launch_file(treeview, "/home/sdethyre/sandbox/auto/./dev");
            break;
        case 10:
            open_web_page(treeview, user_data, "open_ti.py" , combobox);
            break;
        default:
            break;
    }
    element_neon_off(GTK_WIDGET(label7));
}

void generic_quit (GtkTreeView *treeview, const char *pidFilename) {
    save_current_dev(treeview);
    clear_file(pidFilename);
    gtk_main_quit();
}

void on_menu_item_activate_R(GtkMenuItem *menu_item, gpointer user_data) {
    MenuItemData *data         = (MenuItemData *)user_data;
    gint selected_option_index = data->index;
    GtkTreeView *treeview      = data->treeview;
    switch (selected_option_index) {
        case 0:
            launch_file(treeview, "meld .");
            break;
        case 1:
            show_commit_dialog(treeview, NULL, NULL);
            break;
        case 2:
            launch_terminal(treeview, "svn update", "intern_minal");
            break;
        case 3:
            launch_terminal(treeview, "", "merge");
            break;
        case 4:
            delete_branch(treeview, NULL, NULL);
            break;
        case 5:
            launch_terminal(treeview, "svn status --ignore-externals | grep '^M'| sed 's/^M//'", "intern_minal");
            break;
        case 6:
            launch_terminal(treeview, "svn st --ignore-externals", "intern_minal");
            break;
        case 7:
            launch_terminal(treeview, "svn diff", "return_key");
            break;
        case 8:
            launch_terminal(treeview, "svn log", "intern_minal");
            break;
        case 9:
            launch_terminal(treeview, "", "last_commit");
            break;
        case 10:
            show_checkout_dialog(treeview, NULL, NULL);
            break;
        default:
            break;
    }
    element_neon_off(GTK_WIDGET(label9));
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
            open_web_page(treeview, user_data, "open_job.py", combobox);
            break;
        case 1:
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) create_popup_info(treeview);
            break;
        case 2:
            launch_file(treeview, "meld .");
            break;
        case 3:
            open_web_page(treeview, user_data, "open_ti.py" , combobox);
            break;
        case 4:
            launch_file(treeview, "/home/sdethyre/bin/atreal/./testLogs");
            break;
        case 5:
            make_label_blink (neon_blink_timeout_id_n, label5);
            launch_file(treeview, "/home/sdethyre/sandbox/auto/./dev");
            break;
        case 6:
            // Exit
            break;
        case 7:
            launch_terminal(treeview, "svn update", "intern_minal");
            break;
        case 8:
            show_commit_dialog(treeview, NULL, NULL);
            break;
        case 9:
            launch_terminal(treeview, "", "merge");
            break;
        case 10:
            launch_terminal(treeview, "svn status --ignore-externals | grep '^M'| sed 's/^M//'", "intern_minal");
            break;
        case 11:
            launch_terminal(treeview, "svn st --ignore-externals", "intern_minal");
            break;
        case 12:
            launch_terminal(treeview, "svn diff", "return_key");
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
        const gchar *menu_items[] = {"Option 1", "Option 2", "Option 3", "Option 4"};
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

        GtkWidget *menu = gtk_menu_new();
        for (int i = 0; i < G_N_ELEMENTS(menu_items); i++) {
            GtkWidget *menu_item = gtk_menu_item_new_with_label(menu_items[i]);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
        }
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
    return FALSE;
}

gboolean on_treeview_key_press_left(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    switch (event->keyval) { 
        case GDK_KEY_Right: 
            if (widget != NULL) gtk_widget_hide(widget); 
            element_neon_off(GTK_WIDGET(label7));
            break;
        case GDK_KEY_Escape:
            element_neon_off(GTK_WIDGET(label7));
            break;
    }
    return FALSE;
}

gboolean on_treeview_key_press_right(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    switch (event->keyval) { 
        case GDK_KEY_Left: 
            if (widget != NULL) gtk_widget_hide(widget);
            element_neon_off(GTK_WIDGET(label9));
            break;
        case GDK_KEY_Escape:
            element_neon_off(GTK_WIDGET(label9));
            break;
    }
    return FALSE;
}

void create_mini_menu(GtkWidget *menu, GtkWidget *widget,const int num_options, const gchar* array_du_select[], const gchar *sens) {
    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    g_signal_handler_disconnect(treeview, key_press_handler_id);
    menu = gtk_menu_new();
    if      (strcmp(sens, "right") == 0) g_signal_connect(menu, "key-press-event", G_CALLBACK(on_treeview_key_press_right), NULL);
    else if (strcmp(sens, "left")  == 0) g_signal_connect(menu, "key-press-event", G_CALLBACK(on_treeview_key_press_left) , NULL);
    for (int i = 0; i < num_options; i++) {
        GtkWidget *menu_item = gtk_menu_item_new_with_label(array_du_select[i]);
        // Créer et peupler la structure de données personnalisée
        MenuItemData *data = g_new(MenuItemData, 1);
        data->index = i;
        data->treeview = treeview;
        // Connecter le signal "activate" et passer la structure de données personnalisée en tant que user_data
        if      (strcmp(sens, "right") == 0) g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate_R), data);
        else if (strcmp(sens, "left")  == 0) g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate_L), data);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    }
    gtk_widget_show_all(menu);
    if (strcmp(sens, "right") == 0) gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(treeview))), GDK_GRAVITY_EAST, GDK_GRAVITY_WEST, NULL);
    else if (strcmp(sens, "left") == 0) gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(treeview))), GDK_GRAVITY_WEST, GDK_GRAVITY_EAST, NULL);
    
    key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
}

gboolean on_treeview_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    const char *pidFilename = "/home/sdethyre/sandbox/dash_dev_boisson/pid.txt";
    GtkTreeView *treeview = GTK_TREE_VIEW(widget);
    GtkWidget *left_menu = NULL;
    GtkWidget *right_menu = NULL;
    switch (event->keyval) {
        case GDK_KEY_r:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_n, label5);
            launch_terminal(treeview, "cat /home/sdethyre/sandbox/dash_dev_boisson/shortcuts.txt", "intern_minal");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_n:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            launch_file(treeview, "/home/sdethyre/sandbox/auto/./dev");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_m:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            launch_terminal(treeview, "", "merge");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_c:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            show_commit_dialog(treeview, NULL, NULL);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_t:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            open_web_page(treeview, user_data, "open_ti.py" , combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_ISO_Level3_Shift:
            element_neon_on(GTK_WIDGET(label6));
            make_label_blink (neon_blink_timeout_id_alt_gr, label6);
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            launch_file(treeview, "/home/sdethyre/bin/easyTests/./testsDialog");
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_space:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            // make_label_blink (neon_blink_timeout_id_space, label8);
            open_file(treeview, user_data, current_appli);
            open_web_page(treeview, user_data, "open_job.py", combobox);
            open_web_page(treeview, user_data, "open_ti.py" , combobox);
            open_web_page(treeview, user_data, "open_app.py", combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            generic_quit (treeview, pidFilename);
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
            break;
        case GDK_KEY_Right:
            element_neon_on(GTK_WIDGET(label9));
            create_mini_menu(right_menu, widget, num_options_R, array_du_select_R, "right");
            break;
        case GDK_KEY_Control_R:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_ctrl_D, label3);
            launch_test(treeview);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_less:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            make_label_blink (neon_blink_timeout_id_less, label1);
            open_web_page(treeview, user_data, "open_app_no_init.py", combobox);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        case GDK_KEY_Escape:
            generic_quit (treeview, pidFilename);
            break;
        case GDK_KEY_x:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (GTK_IS_WIDGET(popup)) gtk_widget_destroy(popup);
            generic_quit (treeview, pidFilename);
            break;
        case GDK_KEY_w:
            g_signal_handler_disconnect(treeview, key_press_handler_id);
            if (popup != NULL) break;
            create_popup_info(treeview);
            key_press_handler_id = g_signal_connect(treeview, "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);
            break;
        default:
            break;
    }
    return FALSE;
}

// Signal "changed" de la sélection
static void on_selection_changed(GtkTreeSelection *selection, gpointer user_data) {
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
    } 
    else current_index = -1; // Aucune ligne n'est sélectionnée, réinitialisation de l'index
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
    if      (checked_vs)      open_file(treeview, user_data)                             ;
    if      (checked_ti)      open_web_page(treeview, user_data, "open_ti.py" , combobox);
    if      (checked_job)     open_web_page(treeview, user_data, "open_job.py", combobox);
    if      (checked_app)     open_web_page(treeview, user_data, "open_app.py", combobox);
    generic_quit (treeview, pidFilename);
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

int main(int argc, char *argv[]) {
    // Dimensions
    window_width = 550;
    number_of_devs = count_lines_in_file("/home/sdethyre/sandbox/dash_dev_boisson/results.txt");
    basic_height = 100 + 18 * number_of_devs;
    intern_minal_height = 300;

    if (is_instance_running()) {
        // Envoyer le signal SIGUSR1 à la première instance
        int first_instance_pid = get_first_instance_PID();
        if (first_instance_pid != -1) {
            kill(first_instance_pid, SIGUSR1);
            g_spawn_command_line_async("/home/sdethyre/sandbox/dash_dev_boisson/dash_dev", NULL);
            return EXIT_SUCCESS;
        }
    }

    int current_pid = getpid(); // Obtenir le PID de l'application en cours d'exécution
    save_PID_to_file(current_pid);

    char command[256];
    sprintf(command, "bash /home/sdethyre/sandbox/dash_dev_boisson/list_devs");
    int exit_status = system(command);

    if (exit_status == -1) {
        printf("Erreur lors de l'exécution du script");
        return 1;
    }
    else if (!WIFEXITED(exit_status) || WEXITSTATUS(exit_status) != 0) {
        printf("Le script s'est terminé avec une erreur");
        return 1;
    }
    gtk_init(&argc, &argv);

    // Créer la fenêtre principale
    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(GTK_WINDOW(window), "Devs    🔧");
    gtk_window_set_default_size(GTK_WINDOW(window), window_width, basic_height);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit_te), NULL);
    gtk_window_move(GTK_WINDOW(window), 750, 300);
    
    // Créer le modèle de données pour la liste
    GtkListStore *list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

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
            
            if (( g_str_has_prefix(line, "5") || g_str_has_prefix(line, "6") ) && is_valid_line) {
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
        }
        fclose(file);
    }
    // Une ligne vide de + dans la liste
    GtkTreeIter blank_iter;
    gtk_list_store_append(list_store, &blank_iter);
    // Définir les valeurs des colonnes de la nouvelle ligne (en laissant les valeurs vides)
    gtk_list_store_set(list_store, &blank_iter, 0, "", 1, "", -1);

    // Créer le GtkTreeView à partir du modèle de données
    GtkWidget *treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    g_object_unref(list_store);

    // Créer et charger le fournisseur CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "treeview:selected {text-shadow: 0 0 2px #fff, 0 0 4px #fff, 0 0 6px #fff, 0 0 8px #308280, 0 0 20px #308280, 0 0 16px #308280, 0 0 20px #308280; }", -1, NULL);
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen   = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    //▶ ◀
    // Créer une colonne pour afficher le texte
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    // GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("\n  < : Appli\t\t\t      ⏎ : VsCode\t\t\t\t Ctrl D : Test  \n  Alt Gr : Choix test\t\t\t     n : Nouveau\t\t\t\t     x : Quitter\n  ⇦ Debug\t\t\t\t\t━━ : Tout\t\t\t\t\t   SVN ⇨ \n", renderer, "text", 0, NULL);
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    // Définir la taille du GtkTreeView
    GtkRequisition requisition;
    gtk_widget_get_preferred_size(treeview, &requisition, NULL);
    requisition.width = 200; // Modifier la largeur souhaitée
    gtk_widget_set_size_request(treeview, requisition.width, requisition.height);

    // Créer la sélection pour le GtkTreeView
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

    gchar *current_dev = recuperer_elements_droite_string("/home/sdethyre/sandbox/dash_dev_boisson/config.conf", "current_dev:");
    select_row_at_index(GTK_TREE_VIEW(treeview), atoi(current_dev));

    // Connecter l'événement de sélection au callback "open_file"
    g_signal_connect(G_OBJECT(treeview), "row-activated", G_CALLBACK(open_file), combobox);

    // Connecter l'événement de pression des touches du clavier au callback "on_treeview_key_press"
    g_signal_connect(G_OBJECT(treeview), "key-press-event", G_CALLBACK(on_treeview_key_press), NULL);

    // Créer le conteneur vertical (vbox)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // 3 * 3
    label1 = gtk_label_new("< : Appli");
    label2 = gtk_label_new("⏎ : VsCode");
    label3 = gtk_label_new("Ctrl D : Test");
    label4 = gtk_label_new("x : Quitter");
    label5 = gtk_label_new("r : Raccourcis");
    label6 = gtk_label_new("Alt Gr : Choix test");
    label7 = gtk_label_new("⇦ Debug");
    label8 = gtk_label_new("━━ : Tout");
    label9 = gtk_label_new("SVN ⇨");

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

    gtk_container_add(GTK_CONTAINER(vbox), treeview);

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

    check_vs  = gtk_check_button_new_with_label("VsCode");
    gtk_grid_attach(GTK_GRID(grid), check_vs,  0, 2, 1, 1);
    check_ti  = gtk_check_button_new_with_label("Ticket Interne");
    gtk_grid_attach(GTK_GRID(grid), check_ti,  1, 2, 1, 1);
    check_job = gtk_check_button_new_with_label("Job Jenkins");
    gtk_grid_attach(GTK_GRID(grid), check_job, 0, 3, 1, 1);
    check_app = gtk_check_button_new_with_label("Application");
    gtk_grid_attach(GTK_GRID(grid), check_app, 1, 3, 1, 1);

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

    GtkWidget *button = gtk_button_new_with_label("Ouvrir");
    gtk_container_add(GTK_CONTAINER(vbox), button);
    g_signal_connect(button, "clicked", G_CALLBACK(launch), treeview);
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
    gtk_main();
    unlink(LOCK_FILE);
    return 0;
}



void show_test_cases_list_1(GList *test_cases_content) {
    // Créer une nouvelle fenêtre
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Contenu des Test Cases");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    // Créer une boîte verticale (vbox) pour contenir les éléments
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Créer un modèle de texte pour afficher le contenu
    GtkTextBuffer *text_buffer = gtk_text_buffer_new(NULL);

    // Ajouter les lignes à la vue de texte
    GList *iter;
    for (iter = test_cases_content; iter != NULL; iter = g_list_next(iter)) {
        gchar *line = iter->data;
        gtk_text_buffer_insert_at_cursor(text_buffer, line, -1);
    }

    // Créer une vue de texte pour afficher le contenu
    GtkWidget *text_view = gtk_text_view_new_with_buffer(text_buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    // Créer une boîte de défilement
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Afficher la fenêtre
    gtk_widget_show_all(window);
}