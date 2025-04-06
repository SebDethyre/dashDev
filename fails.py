import sys
import requests

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
        found_fail = False
        data = response.json()
        # print(data)
        for index, action in enumerate(data["actions"]):
            if "_class" in action and action["_class"] == "hudson.plugins.robot.RobotBuildAction":
                fail_count = action.get("failCount")
                
                if fail_count is not None:
                    found_fail = True
                print(fail_count)
                break
        if not found_fail:
            if "building" in data and data["building"]:
                print("🟡")
            else:
                print("🔴")
    else:
        print("Job inexistant : erreur", response.status_code)
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_fail_count_for_branch(branch,appli)
