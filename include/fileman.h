#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h> 

#ifndef FILEMAN_H
#define FILEMAN_H

gint count_lines_in_file(const char *filename) ;
gchar* get_elements_at_right_of_string(const char* fichier, const char* chaine) ;
void replace_line(char* filename, char* search_string, char* replace_string) ;
gchar* list_modified_files(const gchar *directory);
int count_modified_files(const gchar *directory);

void open_file(GtkTreeView *treeview, gpointer user_data, const gchar *appli);
void save_PID_to_file(int pid) ;
void save_PID_log_to_file(int pid) ;
void clear_file(const char *filename) ;
void open_web_page(GtkTreeView *treeview, gpointer user_data, const char* fichier, const gchar *appli, GtkWidget *combobox);
char *execute_command(const char *command);
char *execute_command_custom(const char *command) ;
void *execute_command_custom_async(const char *command);

#endif