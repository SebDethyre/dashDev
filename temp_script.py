
import time
from selenium import webdriver

username = "sdethyre"
password = "Spdrmns65,"

driver = webdriver.Chrome("chromedriver")
url = f"https://dev.atreal.fr/projets/opendebitdeboisson-v69123-lyon/interne/tracker/45/poi_issue_view"
driver.get(url)
driver.find_element("id", "__ac_name").send_keys(username)
driver.find_element("id", "__ac_password").send_keys(password)
driver.find_element("name", "submit").click()

# Ne pas fermer le navigateur ici
while True:
    time.sleep(1)
    continue
