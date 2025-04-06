import sys
import requests

def get_change_set_info(branch,appli):
    application =""
    if appli == "opendebitdeboisson":
        application = "openDebitDeBoisson"
    elif appli == "openads":
        application = "openADS"
    elif appli == "openaria":
        application = "openAria"
    elif appli == "openresultat":
        application = "openResultat"
    elif appli == "openlec":
        application = "openElec"
    elif appli == "opencimetiere":
        application = "openCimetiere"
    url = f"https://ci1.atreal.fr/view/P2%20devs/job/{application}__P2_{branch}/lastBuild/api/json"
    response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))
    
    if response.status_code == 200:
        data = response.json()
        # print(data)
        change_set = data.get("changeSet", {})
        items = change_set.get("items", [])

        if items:
            print("\n\tDernier commit :\n")
            for item in items:
                author = item.get("author", {}).get("fullName", "Inconnu")
                message = item.get("msg", "Message non disponible")
                files_changed = item.get("affectedPaths", [])
                revision = item.get("revision", "N/A")

                print(f"\tAuteur        {author[:-4]} ")
                print(f"\tMessage    {message}\t")
                print(f"\tRévision     {revision}\t")

                if len(files_changed) == 1:
                    print(f"\tModifié      {files_changed[0]}\t")
                else:
                    files_changed_str = ', '.join(files_changed)
                    print(f"\tModifiés     {files_changed_str.lstrip(', ')}\t")  # Retirer la virgule au début
                    
                # print("━" * 50)
        else:
            # print("Aucune information sur les changements")
            print("")
    else:
        print("Erreur :", response.status_code)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Veuillez fournir le nom de la branche")
    else:
        appli = sys.argv[1]
        branch = sys.argv[2]
        get_change_set_info(branch,appli)



###  Résultats limités à 4 :

# import sys
# import requests

# def get_change_set_info(branch, num_commits=4):
#     url = f"https://ci1.atreal.fr/view/P2%20devs/job/openDebitDeBoisson__P2_{branch}/lastBuild/api/json"
#     response = requests.get(url, auth=('sdethyre', 'mIeLvWHXVJjwfqp2'))
    
#     if response.status_code == 200:
#         data = response.json()
#         change_set = data.get("changeSet", {})
#         items = change_set.get("items", [])[:num_commits]  # Slice to get only the first n items

#         if items:
#             print("\n\tDerniers commits :\n")
#             for item in items:
#                 author = item.get("author", {}).get("fullName", "Inconnu")
#                 message = item.get("msg", "Message non disponible")
#                 files_changed = item.get("affectedPaths", [])
#                 revision = item.get("revision", "N/A")

#                 print(f"\tAuteur        {author[:-4]} ")
#                 print(f"\tMessage    {message}\t")
#                 print(f"\tRévision     {revision}\t")

#                 if len(files_changed) == 1:
#                     print(f"\tModifié      {files_changed[0]}\t")
#                 else:
#                     files_changed_str = ', '.join(files_changed)
#                     print(f"\tModifiés     {files_changed_str.lstrip(', ')}\t")  # Retirer la virgule au début
                    
#         else:
#             print("")
#     else:
#         print("Erreur :", response.status_code)

# if __name__ == "__main__":
#     if len(sys.argv) != 2:
#         print("Veuillez fournir le nom de la branche")
#     else:
#         branch = sys.argv[1]
#         get_change_set_info(branch, num_commits=4)  # Call the function with the desired number of commits
