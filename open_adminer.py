import sys
import subprocess

def open_web_page(appli):
    # print(page_number,appli)
    application =""
    if appli == "opendebitdeboisson":
        application = "opendebitdeboisson"
    elif appli == "openads":
        application = appli
    elif appli == "opencimetiere":
        application = appli
    else :
        application = appli
    script = f"""
import time
from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import Select
from selenium import webdriver

driver = webdriver.Chrome("chromedriver")
url = f"http://localhost/adminer/"
driver.get(url)
select = Select(driver.find_element(By.XPATH, "//select[@name='auth[driver]']"))
select.select_by_visible_text("PostgreSQL")
driver.find_element("name", "auth[server]").send_keys("localhost")
driver.find_element("name", "auth[username]").send_keys("postgres")

driver.find_element("name", "auth[password]").send_keys("postgres")

driver.find_element("name", "auth[db]").send_keys("{application}")
# time.sleep(1000)
# driver.find_element("type", "submit").click()
driver.find_element(By.XPATH, "//input[@type='submit']").click()
# WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//select[@name='ns']")))
time.sleep(.3)
select = Select(driver.find_element(By.XPATH, "//select[@name='ns']"))
select.select_by_visible_text("{application}")

# Ne pas fermer le navigateur ici
while True:
    time.sleep(1)
    continue
"""
    # Écrire le script dans un fichier temporairez
    script_file = "/home/sdethyre/garbage/temp_script.py"
    with open(script_file, "w") as file:
        file.write(script)

    # Exécuter le script dans un processus séparé en utilisant sys.executable
    subprocess.Popen([sys.executable, script_file])

if __name__ == "__main__":
    # Récupération du numéro de page passé en argument
    if len(sys.argv) > 1:
        appli = sys.argv[1] 
        open_web_page(appli)
    else:
        print("Page number not provided")
