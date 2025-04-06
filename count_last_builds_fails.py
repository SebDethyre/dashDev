# # Avec dates & heures

# import requests
# from concurrent.futures import ThreadPoolExecutor, as_completed
# import datetime
# SESSION = requests.Session()

# def get_json(url):
#     with SESSION.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
#         if response.status_code == 200:
#             return response.json()
#         return None

# def get_build_error_count(console_url):
#     with requests.get(console_url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
#         if response.status_code == 200:
#             return response.text.count("ERROR")
#     return None

# def format_date(date_str):
#     # Convertir la date au format timestamp en utilisant strptime
#     timestamp = datetime.datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')
#     # Formater la date au format français
#     return timestamp.strftime('%d/%m/%Y %H:%M:%S')

# def process_build(build):
#     build_url = build["url"]
#     error_count = get_build_error_count(build_url + "consoleText")
#     if error_count is not None:
#         build_data = get_json(build_url + "api/json")
#         if build_data:
#             fail_count = reformat_data(build_data)
#             start_time = format_date(datetime.datetime.fromtimestamp(build_data["timestamp"] / 1000).strftime('%Y-%m-%d %H:%M:%S'))
#             end_time = format_date(datetime.datetime.fromtimestamp((build_data["timestamp"] + build_data["duration"]) / 1000).strftime('%Y-%m-%d %H:%M:%S'))
#             return {
#                 "build_number": build["number"],
#                 "error_count": error_count,
#                 "reformatted_fail_count": fail_count,
#                 "start_time": start_time,
#                 "end_time": end_time
#             }
#     return None

# def get_builds_with_error_count(job_name):
#     url = f"https://ci1.atreal.fr/job/{job_name}/api/json"
#     data = get_json(url)
#     if data:
#         builds = data.get("builds", [])
#         with ThreadPoolExecutor() as executor:
#             results = list(executor.map(process_build, builds))
#         return [build for build in results if build is not None]
#     return None

# def reformat_data(build_data):
#     for action in build_data["actions"]:
#         if "_class" in action and action["_class"] == "hudson.plugins.robot.RobotBuildAction":
#             fail_count = action.get("failCount")
#             if fail_count is not None:
#                 return fail_count
#     if "building" in build_data and build_data["building"]:
#         return "🟡"
#     return "🔴"

# if __name__ == "__main__":
#     import sys
    
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche en tant qu'argument")
#     else:
#         branch = sys.argv[1]
#         job_name = f"openDebitDeBoisson__P2_{branch}"
#         builds_with_errors = get_builds_with_error_count(job_name)

#         if builds_with_errors is not None:
#             results_text = "\n".join(
#                 f"Build {build['build_number']} - errors: {build['error_count']}, fails: {build['reformatted_fail_count']} "
#                 f"\t\t{build['start_time']}  ⮕  {build['end_time'].split(' ')[1]}"
#                 for build in builds_with_errors
#             )
                        
#             with open("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results", "w") as results_file:
#                 results_file.write(results_text)
            
#             print("Résultats stockés avec succès dans 'last_builds_results'.")
#         else:
#             print("Erreur lors de la récupération des données.")


## Sans dates & heures
# import requests
# from threading import Thread
# from queue import Queue

# SESSION = requests.Session()

# def get_json(url):
#     with SESSION.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
#         if response.status_code == 200:
#             return response.json()
#         return None

# def get_build_error_count(console_url):
#     with requests.get(console_url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
#         if response.status_code == 200:
#             return response.text.count("ERROR")
#     return None

# def process_build(build, result_queue):
#     build_url = build["url"]
#     error_count = get_build_error_count(build_url + "consoleText")
#     if error_count is not None:
#         build_data = get_json(build_url + "api/json")
#         if build_data:
#             fail_count = reformat_data(build_data)
#             result_queue.put({"build_number": build["number"], "error_count": error_count, "reformatted_fail_count": fail_count})

# def get_builds_with_error_count(job_name):
#     url = f"https://ci1.atreal.fr/job/{job_name}/api/json"
#     data = get_json(url)
#     if data:
#         builds = data.get("builds", [])
#         result_queue = Queue()
#         threads = []

#         for build in builds:
#             thread = Thread(target=process_build, args=(build, result_queue))
#             threads.append(thread)
#             thread.start()

#         # Attendre que tous les threads se terminent
#         for thread in threads:
#             thread.join()

#         results = []
#         while not result_queue.empty():
#             results.append(result_queue.get())

#         return results

#     return None

# def reformat_data(build_data):
#     for action in build_data["actions"]:
#         if "_class" in action and action["_class"] == "hudson.plugins.robot.RobotBuildAction":
#             fail_count = action.get("failCount")
#             if fail_count is not None:
#                 return fail_count
#     if "building" in build_data and build_data["building"]:
#         return "🟡"
#     return "🔴"

# if __name__ == "__main__":
#     import sys
    
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche en tant qu'argument")
#     else:
#         branch = sys.argv[1]
#         job_name = f"openDebitDeBoisson__P2_{branch}"
#         builds_with_errors = get_builds_with_error_count(job_name)

#         if builds_with_errors is not None:
#             results_text = "\n".join(
#                 f"Build {build['build_number']} - errors: {build['error_count']}, fails: {build['reformatted_fail_count']}"
#                 for build in builds_with_errors
#             )
            
#             with open("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results", "w") as results_file:
#                 results_file.write(results_text)
            
#             print("Résultats stockés avec succès dans 'last_builds_results'.")
#         else:
#             print("Erreur lors de la récupération des données.")


# # Avec dates & heures & threads
# import requests
# from threading import Thread, Lock
# import datetime

# SESSION = requests.Session()

# def get_json(url):
#     with SESSION.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
#         if response.status_code == 200:
#             return response.json()
#         return None

# def get_build_error_count(console_url):
#     with requests.get(console_url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
#         if response.status_code == 200:
#             return response.text.count("ERROR")
#     return None

# # def format_date(date_str):
# #     timestamp = datetime.datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')
# #     return timestamp.strftime('%d/%m/%Y %H:%M:%S')

# def format_date(date_str):
#     timestamp = datetime.datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')
#     # Utilisez le format '%d/%m/%Y %H:%M' pour exclure les secondes
#     return timestamp.strftime('%d/%m/%Y %H:%M')

# def process_build(build, result_lock, results):
#     build_url = build["url"]
#     error_count = get_build_error_count(build_url + "consoleText")
#     if error_count is not None:
#         build_data = get_json(build_url + "api/json")
#         if build_data:
#             fail_count = reformat_data(build_data)
#             start_time = format_date(datetime.datetime.fromtimestamp(build_data["timestamp"] / 1000).strftime('%Y-%m-%d %H:%M:%S'))
#             end_time = format_date(datetime.datetime.fromtimestamp((build_data["timestamp"] + build_data["duration"]) / 1000).strftime('%Y-%m-%d %H:%M:%S'))

#             with result_lock:
#                 results.append({
#                     "build_number": build["number"],
#                     # "error_count": error_count,
#                     "reformatted_fail_count": fail_count,
#                     "start_time": start_time,
#                     "end_time": end_time
#                 })

# def get_builds_with_error_count(job_name):
#     url = f"https://ci1.atreal.fr/job/{job_name}/api/json"
#     data = get_json(url)
#     if data:
#         builds = data.get("builds", [])
#         result_lock = Lock()
#         results = []

#         threads = []

#         for build in builds:
#             thread = Thread(target=process_build, args=(build, result_lock, results))
#             threads.append(thread)
#             thread.start()

#         for thread in threads:
#             thread.join()

#         # sorted_results = sorted(results, key=lambda x: x["build_number"])
#         sorted_results = sorted(results, key=lambda x: x["build_number"], reverse=True)

#         return sorted_results

#     return None

# def reformat_data(build_data):
#     for action in build_data["actions"]:
#         if "_class" in action and action["_class"] == "hudson.plugins.robot.RobotBuildAction":
#             fail_count = action.get("failCount")
#             if fail_count is not None:
#                 return fail_count
#     if "building" in build_data and build_data["building"]:
#         return "🟡"
#     return "🔴"

# if __name__ == "__main__":
#     import sys
    
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche en tant qu'argument")
#     else:
#         branch = sys.argv[1]
#         job_name = f"openDebitDeBoisson__P2_{branch}"
#         builds_with_errors = get_builds_with_error_count(job_name)

#         if builds_with_errors is not None:
#             results_text = "\n".join(
#                 # f"Build {build['build_number']} - errors: {build['error_count']}, fails: {build['reformatted_fail_count']} "
#                 f"Build {build['build_number']}, fails: {build['reformatted_fail_count']} "
#                 f"\t\t{'      '.join(build['start_time'].split())}  ⮕  {build['end_time'].split(' ')[1]}"
#                 for build in builds_with_errors
#             )
                        
#             with open("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results", "w") as results_file:
#                 results_file.write(results_text)
            
#             print("Résultats stockés avec succès dans 'last_builds_results'.")
#         else:
#             print("Erreur lors de la récupération des données.")



# # Avec dates & heures & threads et sans les secondes
import requests
from threading import Thread, Lock
import datetime

SESSION = requests.Session()

def get_json(url):
    with SESSION.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
        if response.status_code == 200:
            return response.json()
        return None

def get_build_error_count(console_url):
    with requests.get(console_url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2')) as response:
        if response.status_code == 200:
            return response.text.count("ERROR")
    return None

def format_date(date_str):
    timestamp = datetime.datetime.strptime(date_str, '%Y-%m-%d %H:%M:%S')
    return timestamp.strftime('%d/%m/%Y %H:%M')

def process_build(build, result_lock, results):
    build_url = build["url"]
    error_count = get_build_error_count(build_url + "consoleText")
    if error_count is not None:
        build_data = get_json(build_url + "api/json")
        if build_data:
            fail_count = reformat_data(build_data)
            start_time = format_date(datetime.datetime.fromtimestamp(build_data["timestamp"] / 1000).strftime('%Y-%m-%d %H:%M:%S'))
            end_time = format_date(datetime.datetime.fromtimestamp((build_data["timestamp"] + build_data["duration"]) / 1000).strftime('%Y-%m-%d %H:%M:%S'))

            with result_lock:
                results.append({
                    "build_number": build["number"],
                    "reformatted_fail_count": fail_count,
                    "start_time": start_time,
                    "end_time": end_time
                })

def get_builds_with_error_count(job_name):
    url = f"https://ci1.atreal.fr/job/{job_name}/api/json"
    data = get_json(url)
    if data:
        builds = data.get("builds", [])
        result_lock = Lock()
        results = []

        threads = []

        for build in builds:
            thread = Thread(target=process_build, args=(build, result_lock, results))
            threads.append(thread)
            thread.start()

        for thread in threads:
            thread.join()

        sorted_results = sorted(results, key=lambda x: x["build_number"], reverse=True)

        return sorted_results

    return None

def reformat_data(build_data):
    for action in build_data["actions"]:
        if "_class" in action and action["_class"] == "hudson.plugins.robot.RobotBuildAction":
            fail_count = action.get("failCount")
            if fail_count is not None:
                return fail_count
    if "building" in build_data and build_data["building"]:
        return "🟡"
    return "🔴"

if __name__ == "__main__":
    import sys
    
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche en tant qu'argument")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        application =""
        if appli == "opendebitdeboisson":
            application = "openDebitDeBoisson"
        elif appli == "openads":
            application = "openADS"
        job_name = f"{application}__P2_{branch}"
        builds_with_errors = get_builds_with_error_count(job_name)

        if builds_with_errors is not None:
            results_text = "\n".join(
                f"Build {build['build_number']}, fails: {build['reformatted_fail_count']} "
                f"\t\t{'            '.join(build['start_time'].split())}  ⮕  {build['end_time'].split(' ')[1]}"
                for build in builds_with_errors
            )
                        
            with open("/home/sdethyre/sandbox/dash_dev_boisson/last_builds_results", "w") as results_file:
                results_file.write(results_text)
            
            # print("Résultats stockés avec succès dans 'last_builds_results'.")
        else:
            print("Erreur lors de la récupération des données.")
