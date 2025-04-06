import sys
import subprocess
import asyncio
import requests
running = True
def get_last_completed_build_number(branch, appli):
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
        return data.get("number", None)
    else:
        print("Erreur :", response.status_code)
        return None
    
async def open_web_page_async(branch, build_number, appli):
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
    script = f"""
import time
from selenium import webdriver

username = "sdethyre"
password = "mIeLvWHXVJjwfqp2"

driver = webdriver.Chrome("chromedriver")
url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/{build_number}/robot/"

driver.get(url)
driver.find_element("name", "j_username").send_keys(username)
driver.find_element("name", "j_password").send_keys(password)
driver.find_element("name", "Submit").click()

# Ne pas fermer le navigateur ici
while True:
    time.sleep(1)
    continue
"""
    script_file = "/home/sdethyre/garbage/temp_script.py"
    with open(script_file, "w") as file:
        file.write(script)

   # Exécuter le script dans un processus séparé en utilisant asyncio.create_subprocess_shell
    process = await asyncio.create_subprocess_shell(
        f"{sys.executable} {script_file}",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    # Attendre que le processus se termine (peut être remplacé par d'autres actions)
    stdout, stderr = await process.communicate()
    print("Script completed:", stdout.decode(), stderr.decode())

    # # Attendre que le processus se termine (peut être remplacé par d'autres actions)
    # await process.communicate()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    elif len(sys.argv) < 3:
        print("Veuillez fournir le nom de l'appli")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        build_number = get_last_completed_build_number(branch, appli)
        print(build_number)

        if build_number is not None:
            loop = asyncio.get_event_loop()
            try:
                loop.run_until_complete(open_web_page_async(branch, build_number, appli))
            except KeyboardInterrupt:  # Gérer l'interruption par l'utilisateur (fermeture de l'interface graphique)
                running = False
                print("Script stopped by user")
            finally:
                loop.close()  # Fermer correctement la boucle asyncio
        else:
            print("Build number not found")
