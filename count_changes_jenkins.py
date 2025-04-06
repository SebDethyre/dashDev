import sys
import json

def get_commit_count_from_file(file_path):
    try:
        with open(file_path, 'r') as json_file:
            data = json.load(json_file)
        change_set = data.get("changeSet", {})
        items = change_set.get("items", [])
        commit_count = len(items)
        return commit_count
    except FileNotFoundError:
        print("Fichier JSON introuvable")
        return 0
    except KeyError:
        print("Les données JSON ne sont pas au format attendu")
        return 0

if __name__ == "__main__":
    file_path = "/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json"
    commit_count = get_commit_count_from_file(file_path)
    print(commit_count)