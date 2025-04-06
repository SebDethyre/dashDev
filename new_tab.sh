#!/bin/sh

# Récupérer l'ID de la fenêtre active
WID=$(xprop -root | grep "_NET_ACTIVE_WINDOW(WINDOW)" | awk '{print $5}')

# Donner le focus à la fenêtre active
xdotool windowfocus $WID

# Ouvrir une nouvelle tab dans GNOME Terminal
xdotool key ctrl+shift+t

# Récupérer l'ID de la nouvelle fenêtre
NEW_WID=$(xprop -root | grep "_NET_ACTIVE_WINDOW(WINDOW)" | awk '{print $5}')

# Donner le focus à la nouvelle fenêtre
xdotool windowfocus $NEW_WID

# Lancer le script /home/user/script.sh dans la nouvelle tab avec les arguments
gnome-terminal -- bash -c "/home/sdethyre/bin/easyTests/./launchTest_boisson.exp '$1' '$2' '$3'; echo 'Appuyez sur Entrée pour fermer le terminal...'; read"
