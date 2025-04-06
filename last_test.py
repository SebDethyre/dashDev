import sys
import requests
import re

def process_tests_lines(lines):
    processed_lines = []
    for line in lines:
        if "Tests" in line and len(line) >= 6:
            line = line[6:]
            match = re.search(r"^(.*?)(?:::|$)", line)
            if match:
                processed_lines.append(match.group(1))
    return processed_lines

def get_last_tests_line_for_branch(branch,appli):
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
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/lastBuild/consoleText"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))
    if response.status_code == 200:
        lines = response.text.split("\n")
        reversed_lines = reversed(lines)
        last_tests_line = None
        for line in reversed_lines:
            if "Tests" in line:
                last_tests_line = line.strip()
                break
        if last_tests_line:
            processed_lines = process_tests_lines([last_tests_line])
            if processed_lines:
                print(processed_lines[0])
            else:
                print("Aucun résultat")
        else:
            print("Test en préparation")
    else:
        print("Job inexistant : erreur", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]

        get_last_tests_line_for_branch(branch,appli)

