#!/bin/bash
if [[ $# == 1 ]]; then 
    sort "$1" | uniq -c
elif [[ $# == 2 ]]; then
    sort "$1" | uniq -c > "$2"
else 
	echo "Napacni argumenti"
	exit 1 
fi
exit 0
    

