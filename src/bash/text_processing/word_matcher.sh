#!/bin/bash
for (( i=1; i<=$#; i++)); do
    if [ "${!i}" = "-f" ]; then 
        j=$((i+1))
        x="${!j}"
        if [ ! -e $x ]; then #-e preveri ali obstaja file, direcotry... (-f samo za directory)
            >&2 echo "File $x: Not found" #1 standard output, 2 standard error 
            exit 1                       #>&2 preusmeri 1 ali 2 na 2 error output 
        fi
    fi
done
if [ "$1" = "-n" ]; then 
    v=$# #dolzina vseh argumentov
    nizpodani="${!v}" #zadnji argument
else 
    nizpodani=$1
fi
dolz=0
stevila=()
stevila+=(${#nizpodani})
f=0
n=0
p=0
file="besede.txt"
if  [ ! -e "$file" ]; then
    >&2 echo "File $file: Not found"
    exit 1
fi 
if [ "$#" != "0" ]; then 
    for (( i=1; i<=$#; i++)); do
        if [[ "${!i}"  == *"-"* ]] && [[ "${!i}"  == *[a-z]* ]] ; then  #*[a-z]* ali vsebuje crko od a do z 
            if [ "${!i}" = "-n" ]; then
                j=$((i+1))
                x="${!j}"

                ((n++))
                if [[ $x  == *","* ]]; then 
                    IFS="," #Internal field seperator (oznaci kako razdeliti besedo)
                    read -a split <<< $x  #prebere z vhoda $x in in razdeli besedo (-a arry) - razdeli v arry 
                    stevila=()
                    for split1 in "${split[@]}"; do
                        stevila+=($split1)
                    done
                elif [[ $x  == *"-"* ]]; then
                    IFS="-" 
                    read -a split <<< $x 
                    st=${split[0]}
                    st1=${split[1]}
                    stevila=()
                    for (( ik=$st; ik<=$st1; ik++)); do
                        stevila+=($ik)
                    done
                else
                    stevila=()
                    stevila+=($x)
                fi 
            elif [ "${!i}" = "-f" ]; then
                ((f++))
                b=$((i+1))
                file="${!b}"
                
            elif [ "${!i}" = "-p" ]; then
                j=$((i+2))
                k=$((i+1))
                ((p++))
                if [ $p > "1" ]; then 
                    crka2=$crka1
                    poz2=$poz
                    crka1="${!j}"
                    poz="${!k}"
                    poz1=$poz
                else 
                    crka1="${!j}"
                    poz="${!k}"
                    poz1=$poz 
                fi
                ((poz--))
                if ! [[ $poz1 =~ ^[0-9]{,2}$ && $poz1 -ne 0 ]]; then #-p $1 mora biti med 0 in 99 
                    >&2 echo "-p wrong argument (pozicija)" #{,2} pomeni 2 mestno stevilo in $1 nee sme biti 0
                    exit 1
                elif ! [[ $crka1 == [a-z] ]] ; then
                    >&2 echo "-p wrong argument (crka)"
                    exit 1
                fi
            else
                >&2 echo "Napacno stikalo"
                exit 1
            fi
        fi
    done
else
    >&2 echo "No argumentov"
    exit 1
fi
while read line; do
    stevec=0 #steje stevilo enakih crk 
    beseda=$nizpodani

    if { [ "$p" = "0" ] || { [ "$p" = "1" ] && [ "${line:$poz:1}" = "$crka1" ]; }; } || #stikalo p 
        { [ "$p" = "2" ] && [ "${line:$poz:1}" = "$crka1" ] && [ "${line:$poz2:1}" = "$crka2" ]; }; then
       for stev in "${stevila[@]}"; do #stikalo n 
           if [ "${#line}" = "$stev" ] ; then #dolzina besede v .txt = dozini besed, ki jih iscemo (stevila-arry)
                for (( i=0; i<${#line}; i++ )); do
                    for (( h=0; h<${#beseda}; h++ )); do
                        if [ "${line:$i:1}" = "${beseda:$h:1}" ]; then
                             ((stevec++))
                             xj="${beseda:$h:1}" #beseda:index besede:dolzina niza(ko iscemo crke = 1)
                             #xj = pozicija crke v besedi - najdena crka
                             beseda=${beseda/$xj//} #v besedi zamenjamo najdeno crko $xj z /
                             break
                        fi
                    done
                done
                if [ "$stevec" = "${#line}" ]; then
                    echo $line
                fi
            fi
        done
    fi
done < $file
exit 0
