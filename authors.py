import sys
import requests

def get_culprits(branch,appli):
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
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/lastBuild/api/json"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

    if response.status_code == 200:
        data = response.json()
        culprits = data.get("culprits", [])
        culprit_names = [culprit["fullName"] for culprit in culprits]
        
        if culprit_names:
            if len(culprit_names) == 1:
                print("Auteur lié au dev :")
            else:
                print("Auteurs liés au dev :")
            
            for culprit in culprit_names:
                print("\t" + culprit[:-4])
        else:
            # print("Aucun auteur lié au dev identifié")
            print("")
    else:
        print("Erreur :", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_culprits(branch,appli)

