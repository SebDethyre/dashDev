import datetime
import json

def reformat_build_time(json_data):
    timestamp = json_data.get('timestamp', None)
    build_number = json_data.get('number', None)
    
    if timestamp is not None and build_number is not None:
        build_time = datetime.datetime.fromtimestamp(timestamp / 1000.0)
        formatted_time = build_time.strftime('%d/%m/%Y %H:%M:%S')
        return f"\nBuild {build_number}, lancé le {formatted_time[:-9]} à {formatted_time[11:]}"
    else:
        return "Informations manquantes dans la réponse."

if __name__ == "__main__":
    with open("/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json", "r") as json_file:
        data = json.load(json_file)

    reformatted_time = reformat_build_time(data)
    print(reformatted_time)