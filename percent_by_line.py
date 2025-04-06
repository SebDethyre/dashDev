import sys
import requests

def get_percentage_line_count_for_branch(branch, total_lines, appli):
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
        line_count = len(lines)
        percentage = (line_count / total_lines) * 100
        print(f"{percentage:.2f}")
    else:
        # print("Job inexistant : erreur", response.status_code)
        print("Job inexistant")

if __name__ == "__main__":
    total_lines = 1893  # Total de lignes attendues
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_percentage_line_count_for_branch(branch, total_lines, appli)


# def get_percentage_line_count_for_branch(branch, total_lines):
#     url = f"https://ci1.atreal.fr/view/P2%20devs/job/openDebitDeBoisson__P2_{branch}/lastBuild/consoleText"
#     response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

#     if response.status_code == 200:
#         lines = response.text.split("\n")
#         line_count = len(lines)
#         print(line_count)

#         if "Finished: FAILURE" in lines[-1]:
#             line_count -= 65

#         print(line_count)
#         percentage = (line_count / total_lines) * 100
#         print(f"Pourcentage de lignes dans l'output : {percentage:.2f}%")
#     else:
#         print("Job inexistant : erreur", response.status_code)

# if __name__ == "__main__":
#     total_lines = 1893  # Total de lignes attendues
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche en tant qu'argument")
#     else:
#         branch = sys.argv[1]
#         get_percentage_line_count_for_branch(branch, total_lines)