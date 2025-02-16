#!/bin/bash

super=0
if [[ $# < 2 ]]; then
	echo "Napacni argumenti."
	exit 1
fi

while test $# -gt 0
do
    case "$1" in #preberemo $1 argument 
        -u)	
		user=$2 
		shift
		;;
        --superuser)	
		super=1 
		;;
		*)	
		echo "Napacni argumenti."
		exit 1
    	;;
        
    esac
    shift #zamenjamo argument $2 z $1 (ce tega nebi bilo bi brali 1. argument do konca - neskonca zanka)
done



password=""
prompt="Vnesite geslo za uporabnika $user:"
while  read -p "$prompt" -s -n 1 char; do
    if [[ $char == $'\0' ]]; then #'\0' - ascii null char 
        break
    fi
    prompt='*'
    password+="$char"
done
echo
sudo useradd -d /home/$user -m -s /bin/bash -p $(mkpasswd --method=sha-512 $password) $user
status=$?


if [[ $super == 1 ]]; then
    sudo usermod -a -G sudo $user
fi

case $status in
	0)
	echo "Uporabnik $user ustvarjen."
    exit 0
    ;;
    9)
	echo "Uporabnik $user ze obstaja."
 	exit 9
 	;;
 	*)
	echo "Napaka"
	exit 1
	;;
 esac




