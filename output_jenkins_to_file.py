import sys
import requests

def get_fail_count_for_branch(branch, appli):
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
    
    output_file = "/home/sdethyre/sandbox/dash_dev_boisson/output.txt"  # Nom du fichier de sortie

    if response.status_code == 200:
        lines = response.text.split("\n")
        with open(output_file, "w") as file:
            file.write("\n".join(lines))
            # for line in lines:
            #     file.write(line + "\n")
    else:
        print("Job inexistant : erreur ", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_fail_count_for_branch(branch,appli)