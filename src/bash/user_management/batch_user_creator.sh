#!/bin/bash
if [[ "$#" -eq 2 || "$#" -eq 3 ]]; then
    while read line; do
        uporabnik=$(echo $line | cut -f1 -d:)
        geslo=$(echo $line | cut -f2 -d:)
        if [[ $geslo == *"6$"* ]]; then
            if [[ "$#" -eq 2 ]]; then 
                sudo sudo useradd $uporabnik -d $2/$uporabnik -m -p $geslo
            elif [[ "$#" -eq 3 ]]; then 
                useradd $uporabnik -d $2/$uporabnik -m -p $geslo -k $3
            fi
        else 
            echo "Uporabnik $uporabnik ne uporablja funkcijo SHA-512"
        fi
    done < $1
exit 0
else
echo "Error: stevilo argumentov ne ustreza"
fi
exit 1 



