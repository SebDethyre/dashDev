import sys
import json

def get_culprits_from_file(branch, file_path):
    try:
        with open(file_path, 'r') as json_file:
            data = json.load(json_file)

        culprits = data.get("culprits", [])
        culprit_names = [culprit["fullName"] for culprit in culprits]
        
        if culprit_names:
            if len(culprit_names) == 1:
                print("Auteur lié :")
            else:
                print("Auteurs liés :")
            
            for culprit in culprit_names:
                print("\t" + culprit[:-4])
        else:
            print("")
    except FileNotFoundError:
        print("Fichier JSON introuvable")
    except KeyError:
        print("Les données JSON ne sont pas au format attendu")

if __name__ == "__main__":
    file_path = "/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json"
    
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        branch = sys.argv[1]
        get_culprits_from_file(branch, file_path)
