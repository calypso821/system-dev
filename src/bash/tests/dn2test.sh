#!/bin/bash

if ! [ -x piknik.sh ]; then
  echo "Datoteka z imenom piknik.sh ne obstaja ali pa ni izvršljiva."
  exit 1
fi

if ! [ -e besede.txt ]; then
  echo "Ni datoteke besede.txt."
  exit 2
fi


tocke1=0
tocke=0

#v nadaljevanju so primeri testov, ne pa vsi testi, ki bodo uporabljeni pri preverjanju delovanja oddane rešitve

#####################napačno stikalo
izpis=$(./piknik.sh -l 2>&1)
status=$?

#izpis napake (ni važno kam)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))

#ustrezni izhodni status
[ $status -gt 0 ] && tocke1=$(($tocke1+1))

#izpis napake na ustrezen izhod
izpis=$(./piknik.sh -l 2>&1 1>/dev/null)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))
tocke=$((tocke+tocke1))

echo -e "Napaka: neznano stikalo.\t\t$tocke1/3: skupaj=$tocke"


mv besede.txt btemp.txt
tocke1=0
#####################datoteka ne obstaja
izpis=$(./piknik.sh niz 2>&1)
status=$?

#izpis napake (ni važno kam)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))

#ustrezni izhodni status
[ $status -gt 0 ] && tocke1=$(($tocke1+1))

#izpis napake na ustrezen izhod
izpis=$(./piknik.sh niz 2>&1 1>/dev/null)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))
tocke=$((tocke+tocke1))

echo -e "Napaka: ni datoteke besede.txt.\t\t$tocke1/3: skupaj=$tocke"



#####################stikalo f
tocke1=0
izpis=$(./piknik.sh -f btemp.txt 2>&1)
status=$?

#izpis napake (ni važno kam)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))

#ustrezni izhodni status
[ $status -gt 0 ] && tocke1=$(($tocke1+1))

#izpis napake na ustrezen izhod
izpis=$(./piknik.sh -f btemp.txt 2>&1 1>/dev/null)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))
tocke=$((tocke+tocke1))

echo -e "Stikalo -f.\t\t\t\t$tocke1/3: skupaj=$tocke"



#####################premalo argumentov
mv btemp.txt besede.txt
tocke1=0
izpis=$(./piknik.sh 2>&1)
status=$?

#izpis napake (ni važno kam)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))

#ustrezni izhodni status
[ $status -gt 0 ] && tocke1=$(($tocke1+1))

#izpis napake na ustrezen izhod
izpis=$(./piknik.sh 2>&1 1>/dev/null)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))
tocke=$((tocke+tocke1))

echo -e "Napaka: ni argumenta.\t\t\t$tocke1/3: skupaj=$tocke"


#####################test niza
tocke1=0
izpis=$(./piknik.sh tksi 2>\dev\null)
[ "$(echo $izpis)" = "kist skit stik tisk" ] && tocke1=$(($tocke1+3))
tocke=$((tocke+tocke1))
echo -e "Pravilen izpis brez stikal.\t\t$tocke1/3: skupaj=$tocke"

#####################test stikala -n
tocke1=0
izpis=$(./piknik.sh tksi -n 3 2>\dev\null)
[ "$(echo $izpis)" = "iks ikt isk ist kis kit sik sit sti tik tsi" ] && tocke1=$(($tocke1+3))
tocke=$((tocke+tocke1))
echo -e "Pravilen izpis s stikalom -n.\t\t$tocke1/3: skupaj=$tocke"

#####################test stikala -p
tocke1=0
izpis=$(./piknik.sh tksi -p 4 k 2>\dev\null)
[ "$(echo $izpis)" = "stik tisk" ] && tocke1=$(($tocke1+2))
tocke=$((tocke+tocke1))
echo -e "Pravilen izpis s stikalom p.\t\t$tocke1/2: skupaj=$tocke"

#####################test stikala -p, preverjanje če smo podali 2 argumenta in če je 1. števila in 2. znak
tocke1=0
izpis=$(./piknik.sh tksi -p k k 2>\dev\null)
status=$?
#ustrezni izhodni status
[ $status -gt 0 ] && tocke1=$(($tocke1+1))
#izpis napake na ustrezen izhod
izpis=$(./piknik.sh tksi -p k k 2>&1 1>/dev/null)
[ -n "$izpis" ] && tocke1=$(($tocke1+1))
tocke=$((tocke+tocke1))
echo -e "Pravilen format stikala p.\t\t$tocke1/2: skupaj=$tocke"

exit

