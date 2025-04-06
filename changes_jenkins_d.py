import sys
import json
import textwrap

def wrap_string(text, max_width):
    wrapper = textwrap.TextWrapper(width=max_width, expand_tabs=False, replace_whitespace=False)
    wrapped_lines = wrapper.wrap(text)
    wrapped_lines_with_tabs = [wrapped_lines[0] + "\t"] + ['\t\t\t\t' + line for line in wrapped_lines[1:]]
    return '\n'.join(wrapped_lines_with_tabs)

def wrap_words(text, max_width):
    words = text.split(', ')
    wrapped_lines = []
    current_line = words[0]
    for word in words[1:]:
        if len(current_line) + len(word) + 2 <= max_width:
            current_line += f', {word}'
        else:
            wrapped_lines.append(current_line)
            current_line = '\t\t\t\t' + word
    wrapped_lines.append(current_line)
    wrapped_lines_with_tabs = [line + "\t" for line in wrapped_lines]
    return '\n'.join(wrapped_lines_with_tabs)

# Résultats limités à n = 4
def get_change_set_info_from_file(branch, file_path, num_commits=4):  # Ajout de l'argument num_commits
    wrapping = 80  # For both words and files
    try:
        with open(file_path, 'r') as json_file:
            data = json.load(json_file)
        change_set = data.get("changeSet", {})
        items = change_set.get("items", [])[:num_commits]  # Slice to get only the first n items

        if items:
            if len(items) > 1:
                print("\nCommits relevés par Jenkins\n")  # Modifier le message d'en-tête
            else:
                print("\nCommit relevé par Jenkins\n")  # Modifier le message d'en-tête
            for item in items:
                author = item.get("author", {}).get("fullName", "Inconnu")
                message = item.get("msg", "Message non disponible")
                files_changed = item.get("affectedPaths", [])
                revision = item.get("revision", "N/A")

                commit_info = [
                    f"\tAuteur        {author[:-4]}",
                    f"\tMessage    {wrap_string(message, wrapping)}",
                    f"\tRévision     {revision}"
                ]
                if len(files_changed) == 1:
                    commit_info.append(f"\tModifié      {files_changed[0]}")
                else:
                    files_changed_str = ', '.join(files_changed)
                    commit_info.append(f"\tModifiés     {wrap_words(files_changed_str, wrapping).lstrip(', ')}")
                print('\n'.join(commit_info) + '\n')
    except FileNotFoundError:
        print("Fichier JSON introuvable")
    except KeyError:
        print("Les données JSON ne sont pas au format attendu")

if __name__ == "__main__":
    file_path = "/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json"
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        branch = sys.argv[1]
        get_change_set_info_from_file(branch, file_path, num_commits=4)  # Call the function with the desired number of commits


# Tous les résultats :

# def get_change_set_info_from_file(branch, file_path):
#     wrapping = 80 # For both words and files
#     try:
#         with open(file_path, 'r') as json_file:
#             data = json.load(json_file)
#         change_set = data.get("changeSet", {})
#         items = change_set.get("items", [])
#         if items:
#             if len(items) > 1:
#                 print("\nCommits liés :\n")
#             else:
#                 print("\nCommit lié :\n")
#             for item in items:
#                 author = item.get("author", {}).get("fullName", "Inconnu")
#                 message = item.get("msg", "Message non disponible")
#                 files_changed = item.get("affectedPaths", [])
#                 revision = item.get("revision", "N/A")

#                 commit_info = [
#                     f"\tAuteur        {author[:-4]}",
#                     f"\tMessage    {wrap_string(message, wrapping)}",
#                     f"\tRévision     {revision}"
#                 ]
#                 if len(files_changed) == 1:
#                     commit_info.append(f"\tModifié      {files_changed[0]}")
#                 else:
#                     files_changed_str = ', '.join(files_changed)
#                     commit_info.append(f"\tModifiés     {wrap_words(files_changed_str, wrapping).lstrip(', ')}")
#                 print('\n'.join(commit_info) + '\n')
#     except FileNotFoundError:
#         print("Fichier JSON introuvable")
#     except KeyError:
#         print("Les données JSON ne sont pas au format attendu")

# if __name__ == "__main__":
#     file_path = "/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json"
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche")
#     else:
#         branch = sys.argv[1]
#         get_change_set_info_from_file(branch, file_path)
