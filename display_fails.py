import sys
import requests
import re

def get_fail_count_for_branch(branch,appli):
    application =""
    if appli == "opendebitdeboisson":
        application = "openDebitDeBoisson"
    elif appli == "openads":
        application = "openADS"
    elif appli == "openaria":
        application = "openAria"
    elif appli == "openresultat":
        application = "openResultat"
    elif appli == "openlec":
        application = "openElec"
    elif appli == "opencimetiere":
        application = "openCimetiere"
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/lastBuild/consoleText"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

    if response.status_code == 200:
        lines = response.text.split("\n")
        found_fail = False
        output_lines = []  # Créer une liste pour stocker les lignes

        exclude_patterns = [
            "Finished: FAILURE",
            "Build step 'Publish Robot Framework test results' changed build result to FAILURE"
        ]

        for line in lines:
            if any(pattern in line for pattern in exclude_patterns):
                continue  # Exclure les lignes correspondantes
            if "FAIL" in line:
                if "Tests" in line:
                    if re.search(r'Tests\s*\|\s*FAIL\s*\|', line):
                        line = line.strip()
                    else:
                        formatted_line = f"\n\n{'=' * 62}\n{line.strip():>60}\n{'=' * 62}"
                        line = formatted_line
                # Vérifier si la ligne contient uniquement les strings "Tests" et "| FAIL |"
                elif line.strip() == "Tests" or line.strip() == "| FAIL |":
                    continue  # Exclure ces lignes
                output_lines.append(line)  # Ajouter la ligne à la liste
                found_fail = True

        if not found_fail:
            output_lines.append("Aucun fail trouvé")  # Ajouter un message à la liste

        # Imprimer les lignes dans l'ordre inverse
        for line in reversed(output_lines):
            print(line)
    else:
        print("Job inexistant : erreur", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_fail_count_for_branch(branch,appli)



        
# Résultats en sens normal :

# def get_fail_count_for_branch(branch):
#     url = f"https://ci1.atreal.fr/view/P2%20devs/job/openDebitDeBoisson__P2_{branch}/lastBuild/consoleText"
#     response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

#     if response.status_code == 200:
#         lines = response.text.split("\n")
#         found_fail = False
#         for line in lines:
#             if "FAIL" in line:
#                 if "Tests" in line:
#                     if re.search(r'Tests\s*\|\s*FAIL\s*\|', line):
#                         line = line.strip() + "\n"
#                     else:
#                         formatted_line = f"{'=' * 62}\n{line.strip():>60}\n{'=' * 62}\n\n"
#                         line = formatted_line
#                 print(line)
#                 found_fail = True
#         if not found_fail:
#             print("Aucun fail trouvé")
#     else:
#         print("Job inexistant : erreur", response.status_code)

# if __name__ == "__main__":
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche en tant qu'argument")
#     else:
#         branch = sys.argv[1]
#         get_fail_count_for_branch(branch)

