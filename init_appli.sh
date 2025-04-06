# #!/bin/bash

# cd /home/sdethyre/public_html/openads/$1
# cd tests
# . venv p3-omdt && export VENV=True && om-tests -c initenv;

# #!/bin/bash

# cd /home/sdethyre/public_html/openads/$1
# cd tests
# . venv p3-omdt && export VENV=True && expect -c '
# set timeout 5
# spawn om-tests -c initenv
# expect "[sudo] Mot de passe de sdethyre :"
# send "Spdrmns65,\r"
# expect eof
# '


# !/usr/bin/expect -f

password=""
set timeout 5

spawn bash -c ". venv p3-omdt && export VENV=True && om-tests -c initenv"

expect "[sudo] Mot de passe de sdethyre :"
send "$password\r"

expect eof



#!/bin/bash

# cd /home/sdethyre/public_html/openads/$1
# cd tests
# . venv p3-omdt && export VENV=True bash -c && timeout 10s om-tests -c initenv && timeout 10s && echo $password

#!/bin/bash

# cd /home/sdethyre/public_html/openads/$1
# cd tests
# echo "$password"
# . venv p3-omdt && export VENV=True && sudo -S om-tests -c initenv <<< "$password"
