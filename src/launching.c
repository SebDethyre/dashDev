#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h> 
#include <string.h>
// #include <sys/types.h>
#include <sys/wait.h>
// #include "../include/fileman.h" 

void launch_file(GtkTreeView *treeview, gchar *filename, const gchar *appli) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (strcmp(filename,"/home/sdethyre/sandbox/auto_boisson/./dev_boisson") == 0){
        g_spawn_command_line_async(filename, NULL);
    }
    else if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *full_text;
        gtk_tree_model_get(model, &iter, 1, &full_text, -1);
        if (full_text && !g_str_has_suffix(full_text, ".bak")) {

            gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s/%s", appli, full_text);
            printf("directory : %s", directory);
            if (chdir(directory) != 0) {
                g_print("Impossible de changer de répertoire.\n");
                g_free(directory);
                return;
            }
            g_spawn_command_line_async(filename, NULL);
            g_free(directory);
        }
        if (full_text) g_free(full_text);
    }
}
void terminate_process(GPid pid) {
    if (pid > 0) {
        g_spawn_close_pid(pid);
    }
}
void launch_test(GtkTreeView *treeview, const gchar *appli) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    FILE *fp;
    char buffer[1024];

    // Exécute la commande Bash et ouvre un flux pour lire la sortie
    if (strcmp(appli, "opendebitdeboisson") == 0) fp = popen("pgrep -af '/usr/bin/gnome-terminal.real --wait -- bash -c . venv p3-omdt && /home/sdethyre/bin/easyTests/./launchTest_boisson.exp' | grep -v grep | wc -l", "r");
    else fp = popen("pgrep -af '/usr/bin/gnome-terminal.real --wait -- bash -c . venv omdt && /home/sdethyre/bin/easyTests/./launchTest_boisson.exp' | grep -v grep | wc -l", "r");
    if (fp == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du flux.\n");
        return;
    }

    // Lit la sortie de la commande ligne par ligne
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
        int terminalCount = atoi(buffer);
        
        if (terminalCount > 0) {
            // Exécute la commande pour obtenir le dernier PID
            if (strcmp(appli, "opendebitdeboisson") == 0) system("/home/sdethyre/sandbox/dash_dev_boisson/terminate_terminals-p3.sh");
            else system("/home/sdethyre/sandbox/dash_dev_boisson/terminate_terminals.sh");

        }
    }

    // Ferme le flux de la première commande
    pclose(fp);
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *full_text;
        gtk_tree_model_get(model, &iter, 1, &full_text, -1);

        if (full_text && !g_str_has_suffix(full_text, ".bak")) {
            gchar *directory = g_strdup_printf("/home/sdethyre/public_html/%s/%s", appli, full_text);
            if (chdir(directory) != 0) {
                printf("%s", directory);
                g_print("Impossible de changer de répertoire.\n");
                g_free(directory);
                return;
            }
            gchar *test_file;
            gchar *t_command;
            if (strcmp(appli, "openads") == 0) {
                test_file = "current_test_openads.txt";
            }
            if (strcmp(appli, "openaria") == 0) {
                test_file = "current_test_openaria.txt";
            }
            if (strcmp(appli, "opencimetiere") == 0) {
                test_file = "current_test_opencimetiere.txt";
            }
            if (strcmp(appli, "opendebitdeboisson") == 0) {
                test_file = "current_test_openboisson.txt";
            }
            if (strcmp(appli, "openelec") == 0) {
                test_file = "current_test_openelec.txt";
            }
            if (strcmp(appli, "openresultat") == 0) {
                test_file = "current_test_openresultat.txt";
            }
            if (strcmp(appli, "opendebitdeboisson") == 0) {
                t_command = g_strdup_printf("gnome-terminal --wait -- bash -c '. venv p3-omdt && /home/sdethyre/bin/easyTests/./launchTest_boisson.exp \"%s\" \"%s\" \"%s\"; echo \"Appuyez sur Entrée pour fermer le terminal...\"; read'", full_text, appli, test_file);
            }
            else  t_command = g_strdup_printf("gnome-terminal --wait -- bash -c '. venv omdt && /home/sdethyre/bin/easyTests/./launchTest_boisson.exp \"%s\" \"%s\" \"%s\"; echo \"Appuyez sur Entrée pour fermer le terminal...\"; read'", full_text, appli, test_file);
    // t_command = g_strdup_printf("gnome-terminal -- bash -c '. venv omdt && /home/sdethyre/sandbox/dash_dev_boisson/./new_tab.sh \"%s\" \"%s\" \"%s\"; echo \"Appuyez sur Entrée pour fermer le terminal...\"; read'", full_text, appli, test_file);

            // gchar *test_path = g_strdup_printf("/home/sdethyre/sandbox/dash_dev_boisson/testFiles/%s", test_file);
            // gchar *refresh_appli = g_strdup_printf("gnome-terminal --wait -- bash -c 'expect . venv p3-omdt && ~/bin/easyTests/./launch_test.exp %s > /dev/null 2>&1; echo \"Application initialisée...\"; read -n 1' 2>/dev/null", full_text);
            // g_spawn_command_line_async("gnome-terminal -- bash -c '. venv p3-omdt && ~/bin/easyTests/./launch_test.sh; read -s -r -p \"Appuyez sur Entrée pour fermer le terminal...\"'", NULL);
            g_spawn_command_line_async(t_command, NULL); 
            g_free(directory);
        }
        if (full_text) g_free(full_text);
    }
}
