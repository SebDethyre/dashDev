import sys
import subprocess
import time

def open_web_page(complete_dev_name, appli):
    script = f"""
import time
import psutil
from selenium import webdriver

username = "sdethyre"
password = "mIeLvWHXVJjwfqp2"
# sdethyre#LmoFjdfV7eDs

# Use Firefox driver instead of Chrome driver
driver = webdriver.Chrome("chromedriver")
# driver = webdriver.Firefox(executable_path="geckodriver")
url = f"https://ci1.atreal.fr/view/P2%20devs/job/{appli}__P2_{complete_dev_name}/"
driver.get(url)
driver.find_element("name", "j_username").send_keys(username)
driver.find_element("name", "j_password").send_keys(password)
driver.find_element("name", "Submit").click()

# Print the PID of the browser process
print("PID of the Firefox browser:", psutil.Process(driver.service.process.pid).pid)

# Do not close the browser here
while True:
    time.sleep(1)
    continue
"""
    script_file = "/home/sdethyre/garbage/temp_script.py"
    with open(script_file, "w") as file:
        file.write(script)
    process = subprocess.Popen([sys.executable, script_file])

if __name__ == "__main__":
    if len(sys.argv) > 1:
        page_number = sys.argv[1]
        appli = sys.argv[2] 
        open_web_page(page_number, appli)
    else:
        print("Page number not provided")
