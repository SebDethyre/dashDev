import json

def reformat_data(data):
    # reformatted_data = "Données reformatées : " + str(data)
    found_fail = False

    for index, action in enumerate(data["actions"]):
        if "_class" in action and action["_class"] == "hudson.plugins.robot.RobotBuildAction":
            fail_count = action.get("failCount")
            
            if fail_count is not None:
                found_fail = True
            reformatted_data = fail_count
            break
    if not found_fail:
        if "building" in data and data["building"]:
            reformatted_data = "🟡"
        else:
            reformatted_data = "🔴"
    return reformatted_data

if __name__ == "__main__":
    # Lire les données à partir du fichier JSON
    with open("/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json", "r") as json_file:
        data = json.load(json_file)

    reformatted_result = reformat_data(data)
    print(reformatted_result)