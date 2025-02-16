#!/bin/bash
p=-10
s=2
n=3
t=300
izpis=0
if [ "$#" = "2" ]; then #conf file
	while read line; do
	    sw=$(echo $line | cut -f1 -d:) #stikalo (-f = field, -d = delimiter)
	    value=$(echo $line | cut -f2 -d:) #value
	    if [ $sw = "log" ]; then
	    	izpis=1
	    	path=$value
	    elif [ $sw = "n" ]; then
	    	n=$value
		elif [ $sw = "p" ]; then
	    	p=$value
		elif [ $sw = "s" ]; then
	    	s=$value
		elif [ $sw = "t" ]; then
	    	t=$value
    	else
    		break
		fi
	done < $2
fi
nt=$(echo $t"/1000" | bc -l) #pretvori ms vs s (bc- calculator, -l standard math library)
name=$(basename $1)
path1=($(readlink -f $1)) #path do programa podanega v argumentih, -f izpise kanonicno pot
p1=${path1[0]}
st=0
for pid in $(ps -ax -o pid=); do #pid vseh procesov -ax(vsi), -o filter izpisa
	ni=$(ps -p $pid -o nice=) #trenutni nice
	nni=$(($ni + $s)) #trenutnemu pristejemo +e
	renice -n $nni -p $pid &> /dev/null 
done
for pid in $(pgrep $name); do #pid dolocenih procesov $1
	if [ $(readlink -f /proc/$pid/exe) = $p1 ]; then #preveri ali je path executable enak kot path $1
		((st++)) #steje procese
		renice -n $p -p $pid  &> /dev/null 
	fi
done
x=$(pidof -s bash) #pid of bahs
x1=$(ps -p $x -o nice=) #nice of bash 
for ((i=0; i<$(($n-$st)); i++)); do #na novo zazene procese ki se mankajo 
	sudo nice -n $(($p-$x1)) $1& &> /dev/null
	sleep $nt # v t intervalih 
done
echo "Na tem mestu, $(date +"%-d. %-m.") je Zajec Veliki pomagal procesu $p1." #izpis na izhod
if [ $izpis = "1" ]; then 
	echo "Na tem mestu, $(date +"%-d. %-m.") je Zajec Veliki pomagal procesu $p1." > $path #izpis v log file 
fi
exit 0

