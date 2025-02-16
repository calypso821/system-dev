#!/bin/bash

if [[ $(/usr/bin/id -u) -ne 0 ]]; then
    echo "Run me with sudo!"
    exit 2
fi

echo ""
function res {
	p=$1
	pts=$2
	c="$3"
	[ $p -eq $pts ] && echo -e "\e[1m$p/$pts\e[0m | $c" || echo -e "\e[41;1m$p/$pts\e[0m | $c"
}

function printSlow {
	s="$1"
	for (( i=0; i<${#s}; i++ )); do
		sleep 0.02
		if [ "${s:$i:1}" = "|" ]; then
			echo -en "\e[3$(( $RANDOM * 6 / 32767 + 1 ))m${s:$i:1}"
		else
			echo -en "\e[0m\e[1m${s:$i:1}"
		fi
	done
	echo -e "\e[0m"
}

function supressErr {
	exec 3>&2
	exec 2> /dev/null
}

function resumeErr {
	exec 2>&3
}

#########################################################################
#########################################################################
# A = 22 pts (8 extra in hidden tests after submission)
#########################################################################
#########################################################################

allP=0
if [ -x dn3a.sh ]; then

	otherPid=$$
    sudo renice 3 $otherPid > /dev/null
	otherNi1=$(ps -o ni -p $otherPid | tail -1)
	
	supressErr
	#######################
	p=0
	pts=4
	c="Test A-1: Check if additional process created when one exists previously."

	sudo killall -9 sleep &>/dev/null
	sleep infinity & &>/dev/null
	sleep infinity & &>/dev/null
	sudo ./dn3a.sh "/bin/sleep infinity" &>/dev/null
	tmp=$(pgrep sleep | wc -l)
	sudo killall -9 sleep &>/dev/null
	if [ $tmp -eq 3 ]; then
		p=$pts
	fi
	
	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	resumeErr
	
	otherNi2=$(ps -o ni -p $otherPid | tail -1)

	supressErr
	#######################
	p=0
	pts=4
	c="Test A-2: Check if processes being created when none exist previously (conf also supplied)."

	confPath="/tmp/os-dn3.conf"
	logPath="/tmp/os-dn3.log"
	echo -e "log:$logPath\nn:2\np:-10\ns:-1\nt:50" > $confPath
	
	sudo killall -9 sleep &>/dev/null
	sudo ./dn3a.sh "/bin/sleep infinity" $confPath >/tmp/dn3a 2>/dev/null
	output=$(cat /tmp/dn3a)
	tmp=$(pgrep sleep | wc -l)
	#sudo killall -9 sleep &>/dev/null
	if [ $tmp -gt 0 ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	resumeErr
	
	#######################
	p=0
	pts=3
	c="Test A-3: Check if output correct."

	outputTruth="Na tem mestu, $(date +"%-d. %-m.") je Zajec Veliki pomagal procesu /bin/sleep."
	if [ "$output" == "$outputTruth" ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	
	#######################
	p=0
	pts=2
	c="Test A-4: Check if niceness level for target process is OK."

	targetNi=$(ps -o ni -p $(pidof -s sleep) | tail -1)
	otherNi3=$(ps -o ni -p $otherPid | tail -1)
	
	if [ $targetNi -eq -10 ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	
	#######################
	p=0
	pts=3
	c="Test A-5: Check if niceness levels for other processes are OK."

	targetNi=$(ps -o ni -p $(pidof -s sleep) | tail -1)
	otherNi3=$(ps -o ni -p $otherPid | tail -1)
	
	if [ $(( $otherNi1 + 2 )) -eq $otherNi2 ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	
	#######################
	p=0
	pts=4
	c="Test A-6: Check if conf is being parsed correctly (through n parameter)."

	if [ $tmp -eq 2 ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	
	#######################
	p=0
	pts=2
	c="Test A-7: Check if logging works (requires conf parsing to work)."

	outputLog=$(cat $logPath)
	if [[ "$output" == "$outputTruth" && "$outputTruth" == "$outputLog" ]]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################


else
	echo -e "\e[31m/// Script dn3a.sh NOT OK ///\e[0m Check the name and the executable bit. \e[31m///\e[0m"
fi
allPa=$allP

#########################################################################
#########################################################################
# B = 6 pts (4 extra in hidden tests after submission)
#########################################################################
#########################################################################

podatkiA="Ime,Tip,Potovalna hitrost,Datum odhoda,Čas odhoda\nGMX2341,Gomuljka,40km/h,5.5.2019,18:05\nInter-City Slovenia Casanova,Pendolino ETR,65km/h,5.5.2019,10:55\nKTX-6489,TGV,305km/h,3.5.2019,02:05\nITX-MMN4,ITX,290km/h,4.5.2019,17:05\nTGV Réseau,TGV,310km/h,3.5.2019,16:05\n"
podatkiB="Ime,Tip,Potovalna hitrost,Datum odhoda,Čas odhoda\nInter-City Slovenia Casanova,Pendolino ETR,65km/h,5.5.2019,10:55\nITX-MMN4,ITX,290km/h,4.5.2019,17:05\nTGV Réseau,TGV,310km/h,3.5.2019,16:05\nKTX-6489,TGV,305km/h,3.5.2019,20:27\nGMX2341,Gomuljka,40km/h,5.5.2019,19:05\n"
podatkiC="Ime,Tip,Potovalna hitrost,Datum odhoda,Čas odhoda\nInter-City Slovenia Casanova,Pendolino ETR,65km/h,5.5.2019,20:55\nITX-MMN4,ITX,290km/h,4.5.2019,17:05\nTGV Réseau,TGV,310km/h,3.5.2019,16:05\nKTX-6489,TGV,305km/h,3.5.2019,20:27\nGMX2341,Gomuljka,40km/h,5.5.2019,19:05\n"

pathA="/tmp/osPodatkiA.csv"
pathB="/tmp/osPodatkiB.csv"
pathC="/tmp/osPodatkiC.csv"

echo -e $podatkiA > $pathA
echo -e $podatkiB > $pathB
echo -e $podatkiC > $pathC

allP=0
if [ -x dn3b.sh ]; then
	#######################
	p=0
	pts=3
	c="Test B-1: Check the output A vs B."

	./dn3b.sh podatkiA.csv podatkiB.csv &> /dev/null
	status=$?
	
	if [ $status -eq 0 ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################
	
	#######################
	p=0
	pts=3
	c="Test B-2: Check the output B vs C."

	./dn3b.sh podatkiB.csv podatkiC.csv &> /dev/null
	status=$?
	
	if [ $status -eq 1 ]; then
		p=$pts
	fi

	res $p $pts "$c"
	allP=$(($allP+$p))
	#######################

else
	echo -e "\e[31m/// Script dn3b.sh NOT OK ///\e[0m Check the name and the executable bit. \e[31m///\e[0m"
fi
allPb=$allP

grande=$(($allPa + $allPb))

echo -e "\n\tHomework 1-A: \e[1m$allPa\e[0m/22 (extra 8 you can get after the submission)"
echo -e "\tHomework 1-B: \e[1m$allPb\e[0m/6 (extra 4 you can get after the submission)"
echo -e "\n\e[94m/// GRAND TOTAL  ///\e[0m\e[1m $grande\e[0m/28 \e[94m///\e[0m\n"
echo -e "\nAfter the submission you can get up to 12 extra points on hidden tests (all together 28+12=40). And in the classroom during the defense you can get up to 60.\n"

rm "/tmp/dn3a" "$confPath" "$logPath" "$pathA" "$pathB" "$pathC"

if [ $grande -eq 28 ]; then
	printSlow "|||||||||||||||||||||||||||||||||||||"
	printSlow "|||||||||||||||||||||||||||||||||||||"
	printSlow "||||||||||||| B R A V O |||||||||||||"
	printSlow "|||||||||||||||||||||||||||||||||||||"
	printSlow "|||||||||||||||||||||||||||||||||||||"
	printSlow "https://bit.ly/30ectVh"
	echo ""
fi
