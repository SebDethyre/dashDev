import sys

def get_percentage_line_count_from_file(branch, total_lines, file_path):
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
            line_count = len(lines)
            percentage = (line_count / total_lines) * 100
            print(f"{percentage:.2f}")
    except FileNotFoundError:
        print("Fichier introuvable")

if __name__ == "__main__":
    total_lines = 1893  # Total de lignes attendues
    file_path = "/home/sdethyre/sandbox/dash_dev_boisson/output.txt"
    
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        branch = sys.argv[1]
        get_percentage_line_count_from_file(branch, total_lines, file_path)
