import sys
import subprocess

def open_web_page(page_number, appli):
    # print(page_number,appli)
    application =""
    if appli == "opendebitdeboisson":
        application = "opendebitdeboisson-v69123-lyon"
    elif appli == "openads":
        application = appli
    elif appli == "opencimetiere":
        application = appli
    else :
        application = appli
    script = f"""
import time
from selenium import webdriver

username = "sdethyre"
password = "Spdrmns65,"

driver = webdriver.Chrome("chromedriver")
url = f"https://dev.atreal.fr/projets/{application}/interne/tracker/{page_number}/poi_issue_view"
driver.get(url)
driver.find_element("id", "__ac_name").send_keys(username)
driver.find_element("id", "__ac_password").send_keys(password)
driver.find_element("name", "submit").click()

# Ne pas fermer le navigateur ici
while True:
    time.sleep(1)
    continue
"""

    # Écrire le script dans un fichier temporaire
    script_file = "/home/sdethyre/garbage/temp_script.py"
    with open(script_file, "w") as file:
        file.write(script)

    # Exécuter le script dans un processus séparé en utilisant sys.executable
    subprocess.Popen([sys.executable, script_file])

if __name__ == "__main__":
    # Récupération du numéro de page passé en argument
    if len(sys.argv) >= 3:
        appli = sys.argv[1]
        page_number = sys.argv[2]
        print(appli)
        open_web_page(page_number, appli)
    else:
        print("Page number not provided")
