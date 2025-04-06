import json
import sys
import time

def calculate_progress(timestamp_milliseconds, estimated_duration_milliseconds):
    current_timestamp_milliseconds = int(time.time() * 1000)
    time_elapsed_milliseconds = current_timestamp_milliseconds - timestamp_milliseconds
    progress_percentage = (time_elapsed_milliseconds / estimated_duration_milliseconds) * 100
    if progress_percentage > 100:
        progress_percentage = 100
    return progress_percentage

def reformat_data(data):
    timestamp_milliseconds = data["timestamp"]
    estimated_duration_milliseconds = data["estimatedDuration"]
    
    progress_percentage = calculate_progress(timestamp_milliseconds, estimated_duration_milliseconds)
    reformatted_data = f"{progress_percentage:.2f}"
    return reformatted_data

if __name__ == "__main__":
    # Lire les données à partir du fichier JSON
    file_path = "/home/sdethyre/sandbox/dash_dev_boisson/jenkins_data_last_build.json"
    try:
        with open(file_path, "r") as json_file:
            data = json.load(json_file)
    except FileNotFoundError:
        print("Fichier JSON introuvable")
        sys.exit(1)

    reformatted_result = reformat_data(data)
    print(reformatted_result)
