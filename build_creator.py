import sys
import requests

def get_build_creator(branch,appli):
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
        first_line = response.text.split("\n")[0]
        if first_line.strip() == "Lancé par une alarme périodique":
            modified_line = first_line.replace("Lancé par", "par")
            print(modified_line)
        elif first_line.strip() == "Lancé par un changement dans la base de code":
            modified_line = first_line.replace("Lancé par", "par")
            print(modified_line)
        else:
            modified_line = first_line.replace("Démarré par l'utilisateur", "par")
            print(modified_line[:-4])
    else:
        print("Job inexistant : erreur", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche en tant qu'argument")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_build_creator(branch,appli)
