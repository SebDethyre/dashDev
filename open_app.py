import sys
import subprocess

def open_web_page(appli, user):
    script = f"""
import time
from selenium import webdriver
from selenium.webdriver.firefox.firefox_binary import FirefoxBinary
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC

username = "{user}"
password = "{user}"

# Spécifier l'emplacement du binaire Firefox
firefox_binary = FirefoxBinary('/home/sdethyre/firefoxes/basic/firefox/firefox-bin')

# Initialiser le driver avec le binaire Firefox
driver = webdriver.Firefox(firefox_binary=firefox_binary, executable_path="geckodriver")
url = f"http://localhost/{appli}/app/index.php?module=dashboard"
driver.get(url)
WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//input[@name='login']"))).send_keys("{user}")
WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//input[@name='password']"))).send_keys("{user}")

driver.find_element(By.NAME, "login.action.connect").click()

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
    if len(sys.argv) > 1:
        appli = sys.argv[1] if len(sys.argv) > 1 else "openads"
        user = sys.argv[2] if len(sys.argv) > 2 else "admin"
        open_web_page(appli,user)
    else:
        print("Page number not provided")

# driver = webdriver.Chrome("chromedriver")