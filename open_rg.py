import sys
import subprocess

def open_web_page(user):
    
    script = f"""
import time
from selenium import webdriver
from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.support import expected_conditions as EC
username = "{user}"
password = "{user}"

driver = webdriver.Chrome("chromedriver")
url = f"https://atreal.cloud.mattermost.com/boards/team/93d6r1r8jb8i3p9bs8pjmjqh8y/bmowuucfj4iddze918iy1yprx9r/vr78jfxdx9iy8pp5t5q3nsdsxpr"
driver.get(url)
# driver.find_element("class", "external-login-button-label").click()
e_mail="sdethyre@atreal.fr"

WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//span[@class='external-login-button-label']"))).click()
WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//input[@name='identifier']"))).send_keys(e_mail)
WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//span[@class='VfPpkd-vQzf8d']"))).click()
# driver.find_element("class", "VfPpkd-vQzf8dd").click()
# driver.find_element("name", "login").send_keys(username)
# driver.find_element("name", "password").send_keys(password)
# driver.find_element("name", "login.action.connect").click()

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
    if len(sys.argv) > 0:
        # page_number = sys.argv[1]
        user = sys.argv[1] if len(sys.argv) > 1 else "admin"
        open_web_page(user)
    else:
        print("Page number not provided")
