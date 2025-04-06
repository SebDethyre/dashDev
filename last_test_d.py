import re

def process_tests_lines(lines):
    processed_lines = []
    for line in lines:
        if "Tests" in line and len(line) >= 6:
            line = line[6:]
            match = re.search(r"^(.*?)(?:::|$)", line)
            if match:
                processed_lines.append(match.group(1))
    return processed_lines

def get_last_tests_line_from_file(file_path):
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
            reversed_lines = reversed(lines)
            last_tests_line = None
            for line in reversed_lines:
                if "Tests" in line:
                    last_tests_line = line.strip()
                    break
            if last_tests_line:
                processed_lines = process_tests_lines([last_tests_line])
                if processed_lines:
                    print(processed_lines[0])
                else:
                    print("Aucun résultat")
            else:
                print("Test en préparation")
    except FileNotFoundError:
        print("Fichier introuvable")

if __name__ == "__main__":
    get_last_tests_line_from_file("/home/sdethyre/sandbox/dash_dev_boisson/output.txt")
