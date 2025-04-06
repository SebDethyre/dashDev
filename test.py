import sys
import requests

def get_last_completed_build_number(branch,appli):
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
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/api/json"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

    if response.status_code == 200:
        data = response.json()
        last_completed_build_number = data.get('lastCompletedBuild', {}).get('number')
        if last_completed_build_number is not None:
            print(f"Numéro du dernier build complété pour la branche {branch}: {last_completed_build_number}")
        else:
            print("Aucun build complété trouvé pour la branche.")
    else:
        print("Job inexistant : erreur", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        branch = sys.argv[1]
        appli = sys.argv[2]
        get_last_completed_build_number(branch,appli)
