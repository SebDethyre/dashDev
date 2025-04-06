import requests
import sys

def trigger_build(appli, branch):
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

    url_jenkins = f"https://ci1.atreal.fr/view/P2%20devs"

    nom_job = f"{application}__P2_{branch}"
    utilisateur = "sdethyre"
    mot_de_passe = "mIeLvWHXVJjwfqp2"

    url_api = f"{url_jenkins}/job/{nom_job}/build"
    auth = (utilisateur, mot_de_passe)
    # print (url_api, " ", auth)
    response = requests.post(url_api, auth=auth)

    if response.status_code == 201:
        print(f"Le build du job '{nom_job}' a été déclenché avec succès.")
    else:
        print(f"Échec du déclenchement du build. Code de statut : {response.status_code}")
        print(response.text)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Veuillez fournir le nom de l'application et le nom de la branche.")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        trigger_build(appli, branch)
