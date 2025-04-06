import requests
from   threading import Thread, Lock
import datetime
import sys
from concurrent.futures import ThreadPoolExecutor
SESSION = requests.Session()

user='sdethyre'
password='mIeLvWHXVJjwfqp2'

def get_json(url):
    with SESSION.get(url, auth=(user, password)) as response:
        if response.status_code == 200:
            return response.json()
        return None

def get_build_error_count(console_url):
    with requests.get(console_url, auth=('sdethyre', password)) as response:
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

def get_builds_with_error_count(self, job_name):
    url = f"https://ci1.atreal.fr/job/{job_name}/api/json"
    data = self.get_json(url)

    if data:
        builds = data.get("builds", [])
        result_lock = Lock()
        results = []

        def process_build(build):
            self.process_build(build, result_lock, results)

        with ThreadPoolExecutor(max_workers=5) as executor:
            executor.map(process_build, builds)

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
        return "C"
    return "X"

if __name__ == "__main__":    
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
            for build in builds_with_errors:
                print(f"Build: {build['build_number']}            fails: {build['reformatted_fail_count']} "
                      f"\t\t{'            '.join(build['start_time'].split())}  ⮕  {build['end_time'].split(' ')[1]}")
            print("\nX : incomplet       C : en cours\n")
        else:
            print("Erreur lors de la récupération des données.")
