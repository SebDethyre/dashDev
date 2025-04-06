import requests
import sys
import datetime

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
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/lastBuild/api/json"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

    if response.status_code == 200:
        data = response.json()
        print (data)
        timestamp = data.get('timestamp', None)
        if timestamp is not None:
            build_time = datetime.datetime.fromtimestamp(timestamp / 1000.0)
            formatted_time = build_time.strftime('%d/%m/%Y %H:%M:%S')
            print("\nBuild lancé le", formatted_time[:-9], "à", formatted_time[11:] )
        else:
            print("Timestamp not found in response.")
        
    else:
        print("Job inexistant : erreur", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_fail_count_for_branch(branch,appli)