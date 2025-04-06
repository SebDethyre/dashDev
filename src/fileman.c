#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h> 
#include <string.h>
#include <stdbool.h>
// #include <sys/types.h>
#include <sys/wait.h>
#include "../include/fileman.h" 

#define MAX_BUFFER_SIZE 1024

gint count_lines_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return -1; // Indication d'erreur
    }
    int lineCount = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            lineCount++;
        }
    }
    fclose(file);
    return lineCount;
}

gchar* get_elements_at_right_of_string(const char* fichier, const char* chaine) {
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

/// @brief Replaces in a file a searched line by another
/// @param filename 
/// @param search_string 
/// @param replace_string 
void replace_line(char* filename, char* search_string, char* replace_string) {
    char temp_filename[] = "temp.txt";
    char buffer[1000];
    FILE *fp_in, *fp_out;
    int found = 0;

    fp_in = fopen(filename, "r");
    if (fp_in == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier.\n");
        return;
    }
    fp_out = fopen(temp_filename, "w");
    if (fp_out == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier temporaire.\n");
        fclose(fp_in);
        return;
    }
    while (fgets(buffer, 1000, fp_in) != NULL) {
        if (strstr(buffer, search_string) != NULL) {
            fprintf(fp_out, "%s\n", replace_string);
            found = 1;
        } else {
            fprintf(fp_out, "%s", buffer);
        }
    }
    fclose(fp_in);
    fclose(fp_out);

    if (found) {
        remove(filename);
        rename(temp_filename, filename);
        // printf("La ligne a été remplacée avec succès.\n");
    } else {
        remove(temp_filename);
        printf("La chaîne de caractères n'a pas été trouvée dans le fichier.\n");
    }
}

gchar* list_modified_files(const gchar *directory)
{
    // Compose the svn status command
    char command[512];
    snprintf(command, sizeof(command), "svn status %s | grep '^M'", directory);

    // Open a pipe to execute the command and capture its output
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        // Failed to execute the command
        return NULL;
    }

    // Initialize a string to store modified files
    GString* modified_files_str = g_string_new(NULL);

    // Read the output of the command line by line
    char line[256];
    gboolean first_file = TRUE;
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // Remove leading and trailing whitespace
        gchar *trimmed_line = g_strstrip(line);

        // Find the position of the fifth '/' character in the line
        char *slash_position = trimmed_line;
        int slash_count = 0;
        while (*slash_position)
        {
            if (*slash_position == '/')
            {
                slash_count++;
                if (slash_count == 6)
                {
                    break;
                }
            }
            slash_position++;
        }

        if (slash_count == 6)
        {
            // Extract the path after the fifth '/'
            gchar *path_after_fifth_slash = slash_position + 1;

            // Add a comma and a space before the file name (except for the first file)
            if (!first_file)
            {
                g_string_append(modified_files_str, ", ");
            }
            else
            {
                first_file = FALSE;
            }

            // Append the modified file to the string
            g_string_append(modified_files_str, path_after_fifth_slash);
        }
    }

    // Close the pipe
    pclose(fp);

    // Convert the GString to a gchar* and free the GString
    gchar* tmp_modified_files = g_string_free(modified_files_str, FALSE);
    // gchar* modified_files = g_strdup_printf("%s", tmp_modified_files);
    // return modified_files;
    return tmp_modified_files;
}


int count_modified_files(const gchar *directory)
{
    // Compose the svn status command
    char command[512];
    snprintf(command, sizeof(command), "svn status %s | grep '^M' | wc -l", directory);
    // Open a pipe to execute the command and capture its output
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        // Failed to execute the command
        return -1;
    }
    // Read the output of the command
    char output[128];
    if (fgets(output, sizeof(output), fp) == NULL)
    {
        // Failed to read the output
        pclose(fp);
        return -1;
    }
    // Close the pipe
    pclose(fp);
    // Convert the output to an integer count
    int count = atoi(output);
    return count;
}

void open_file(GtkTreeView *treeview, gpointer user_data, const gchar *appli)
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *full_text;
        gtk_tree_model_get(model, &iter, 1, &full_text, -1);

        if (full_text && !g_str_has_suffix(full_text, ".bak"))
        {
            gchar *command = g_strdup_printf("code --new-window ~/public_html/%s/%s", appli, full_text);
            system(command);
            g_free(command);
        }
        if (full_text)
            g_free(full_text);
    }
}

void save_PID_to_file(int pid) {
    FILE *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/pid.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d\n", pid);
        fclose(file);
    }
}
void save_PID_log_to_file(int pid) {
    FILE *file = fopen("/home/sdethyre/sandbox/dash_dev_boisson/pid_log.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d\n", pid);
        fclose(file);
    }
}

void clear_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fclose(file);
}

void open_web_page(GtkTreeView *treeview, gpointer user_data, const char* fichier, const gchar *appli, GtkWidget *combobox)
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *command;
    const gchar *profile_role = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox));

    if (strcmp(fichier, "open_app_no_init.py") == 0) {
        command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_app_no_init.py %s %s", appli, profile_role);
        system(command);
        g_free(command);
    }
    else if (strcmp(fichier, "open_adminer.py") == 0) {
        command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_adminer.py %s", appli);
        system(command);
        g_free(command);
    }
    else if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *full_text;
        gtk_tree_model_get(model, &iter, 1, &full_text, -1);

        if (full_text && !g_str_has_suffix(full_text, ".bak")) {
            if (strcmp(fichier, "open_ti.py") == 0) {
                gchar *substring = full_text + 15;
                if(g_str_has_suffix(full_text, "develop")) return;
                if(g_str_has_suffix(full_text, "refonte-graphique")) return;
                if(g_str_has_prefix(full_text, "rg-")) {
                    return;
                }
                else {
                    strcat(substring, ":");
                    
                    gchar* number = get_elements_at_right_of_string("/home/sdethyre/sandbox/dash_dev_boisson/ticket_numbers.txt", substring);
                    printf("number %s",number);
                    command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_ti.py %s %s", appli, number);
                }
            }
            else if (strcmp(fichier, "open_job.py") == 0) command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_job.py %s %s", full_text, appli);
            else if (strcmp(fichier, "open_job_report.py") == 0) {
                command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_job_report.py %s %s", full_text, appli);
                // printf("sub:%s",full_text);
                // Utiliser subprocess pour exécuter le script en arrière-plan
                int pid = fork();
                if (pid == -1) {
                    g_printerr("Erreur lors de la création du processus fils.\n");
                    g_free(command);
                    // g_free(escaped_to_copy);
                    return;
                }

                if (pid == 0) {
                    // Processus fils
                    GError *error = NULL;
                    if (!g_spawn_command_line_async(command, &error)) {
                        g_printerr("Erreur lors de l'exécution de la commande : %s\n", error->message);
                        g_error_free(error);
                    }
                    _exit(0);
                } else {
                    // Processus parent
                    int status;
                    waitpid(pid, &status, 0);
                }
                g_free(command);
                return;
            }
            else if (strcmp(fichier, "open_job_results.py") == 0) {
                command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_job_results.py %s", full_text);
                printf("sub:%s",full_text);
                // Utiliser subprocess pour exécuter le script en arrière-plan
                int pid = fork();
                if (pid == -1) {
                    g_printerr("Erreur lors de la création du processus fils.\n");
                    g_free(command);
                    // g_free(escaped_to_copy);
                    return;
                }

                if (pid == 0) {
                    // Processus fils
                    GError *error = NULL;
                    if (!g_spawn_command_line_async(command, &error)) {
                        g_printerr("Erreur lors de l'exécution de la commande : %s\n", error->message);
                        g_error_free(error);
                    }
                    _exit(0);
                } else {
                    // Processus parent
                    int status;
                    waitpid(pid, &status, 0);
                }
                g_free(command);
                return;
            }
            else if (strcmp(fichier, "open_app.py") == 0) {
                // gchar *refresh_appli = g_strdup_printf("gnome-terminal --wait -- bash -c 'bash /home/sdethyre/sandbox/dash_dev_boisson/init_appli.sh %s > /dev/null 2>&1; echo \"Application initialisée...\"; read -n 1' 2>/dev/null", full_text);
                // gchar *refresh_appli = g_strdup_printf("gnome-terminal --wait -- bash -c 'expect /home/sdethyre/sandbox/dash_dev_boisson/init_appli.exp %s %s> /dev/null 2>&1; echo \"Appuyer sur une touche pour continuer...\"; read -n 1' 2>/dev/null", appli, full_text);
                gchar *refresh_appli = g_strdup_printf("gnome-terminal --wait -- bash -c 'expect /home/sdethyre/sandbox/dash_dev_boisson/init_appli.exp \"%s\" \"%s\"; echo \"Appuyer sur une touche pour continuer...\"; read -n 1' 2>/dev/null", appli, full_text);

                pid_t appli_pid = fork();
                if (appli_pid == 0) {
                    system(refresh_appli);
                    exit(0);
                } else if (appli_pid > 0) {
                    waitpid(appli_pid, NULL, 0);
                } else {
                    fprintf(stderr, "Failed to fork process for application initialization\n");
                }
                command = g_strdup_printf("python3.10 /home/sdethyre/sandbox/dash_dev_boisson/open_app.py %s %s",appli, profile_role);
            }
            system(command);
            g_free(command);
        }
        if (full_text)
            g_free(full_text);
    }
}

char *execute_command(const char *command) {
    char buffer[MAX_BUFFER_SIZE];
    char *result = NULL;

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        return NULL;
    }

    size_t total_size = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t line_length = strlen(buffer);
        result = realloc(result, total_size + line_length + 1);
        if (result == NULL) {
            perror("realloc failed");
            break;
        }
        strcpy(result + total_size, buffer);
        total_size += line_length;
    }

    if (pclose(fp) == -1) {
        perror("pclose failed");
    }
    return result;
}

char *execute_command_custom(const char *command) {
    char buffer[MAX_BUFFER_SIZE];
    char *result = NULL;

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        return NULL;
    }

    size_t total_size = 0;
    size_t last_non_empty_line = 0;  // Nouvelle variable pour suivre la dernière ligne non vide

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strcmp(buffer, "------------------------------------------------------------------------\n") != 0) {
            size_t line_length = strlen(buffer);
            result = realloc(result, total_size + line_length + 1);
            if (result == NULL) {
                perror("realloc failed");
                break;
            }
            strcpy(result + total_size, buffer);
            total_size += line_length;

            // Mettre à jour last_non_empty_line si la ligne n'est pas vide
            if (line_length > 1) {
                last_non_empty_line = total_size;
            }
        }
    }

    if (pclose(fp) == -1) {
        perror("pclose failed");
    }

    // Terminer la chaîne à la dernière ligne non vide
    if (last_non_empty_line < total_size) {
        result[last_non_empty_line] = '\0';
    }

    return result;
}

#define MAX_BUFFER_SIZE 1024

void *execute_command_custom_async(const char *command) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return NULL;
    }

    if (pid == 0) { // Child process
        // Execute the command
        FILE *fp = popen(command, "r");
        if (fp == NULL) {
            perror("popen failed");
            exit(1);
        }

        char buffer[MAX_BUFFER_SIZE];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Process the output asynchronously
            // You can add your logic here to handle the output
        }

        if (pclose(fp) == -1) {
            perror("pclose failed");
        }

        exit(0);
    } else { // Parent process
        // Parent process continues here
        // You can perform other tasks while the child process is running
    }
    return NULL;
}