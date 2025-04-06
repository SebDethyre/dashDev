import json

def reformat_build_creator(json_data):
    causes = json_data.get("actions", [])[0].get("causes", [])
    
    for cause in causes:
        if "shortDescription" in cause:
            description = cause["shortDescription"]
            if description == "Lancé par une alarme périodique":
                return description.replace("Lancé par", "par")
            elif description == "Lancé par un changement dans la base de code":
                return description.replace("Lancé par", "par")
            elif description.startswith("Démarré par l'utilisateur"):
                return description[len("Démarré par l'utilisateur"):-4]
    
    return "Aucune information disponible"

if __name__ == "__main__":
    # Simulate reading data from a JSON file (replace this with your actual API response)
    with open("/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json", "r") as json_file:
        json_data = json.load(json_file)

    reformatted_line = reformat_build_creator(json_data)
    print(reformatted_line)