import sys
import time
import requests

def calculate_progress(timestamp_milliseconds, estimated_duration_milliseconds):
    current_timestamp_milliseconds = int(time.time() * 1000)
    time_elapsed_milliseconds = current_timestamp_milliseconds - timestamp_milliseconds
    progress_percentage = (time_elapsed_milliseconds / estimated_duration_milliseconds) * 100
    if progress_percentage > 100:
        progress_percentage = 100
    return progress_percentage

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
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/lastBuild/api/json"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))

    if response.status_code == 200:
        data = response.json()
        # Obtention du timestamp et de l'estimation de durée depuis les données
        timestamp_milliseconds = data["timestamp"]
        estimated_duration_milliseconds = data["estimatedDuration"]
        
        progress_percentage = calculate_progress(timestamp_milliseconds, estimated_duration_milliseconds)
        print(f"{progress_percentage:.2f}")
    else:
        print("Job inexistant : erreur", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche en tant qu'argument")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_fail_count_for_branch(branch,appli)
