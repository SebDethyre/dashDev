import sys

def get_fail_count_from_file(file_path):
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
            fail_count = 0

            for line in lines:
                if "| FAIL |" in line and "Tests" not in line:
                    fail_count += 1
            print(fail_count)
    except FileNotFoundError:
        print("Fichier introuvable")

if __name__ == "__main__":
    file_path = "/home/sdethyre/sandbox/dash_dev_boisson/output.txt"
    get_fail_count_from_file(file_path)
