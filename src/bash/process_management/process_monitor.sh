#!/bin/bash
tab_m=();
tab_m+=("\e[38;5;16m")
tab_m+=("\e[38;5;17m")
tab_m+=("\e[38;5;18m")
tab_m+=("\e[38;5;19m")
tab_m+=("\e[38;5;20m")
tab_m+=("\e[38;5;21m")

reset="\e[38;5;231m"
red="\e[38;5;1m"
clear
while :; do
	read -p "Vnesite PID: " x
	osvezi () {
		clear
		if ps -p $x >&/dev/null; then  #poglej ce je v PID veljaven 
			#tabela procesov
			tab=();
			#nice procesa x 
			cnice=$(ps -o nice= -p $x)

			#dodjanje starsev z metodo pstree (izpis vseh podstarsev)
			# sep='---'
			# if [[ "$x" == 1 ]]; then
			# 	tab+=("$(ps -p 1 -o comm=)(1)")
			# else
			# 	str="$(pstree -spA $x)$sep"

			# 	while [[ $str ]]; do
			# 		tab+=("${str%%"$sep"*}")
			# 		str=${str#*"$sep"}
			# 	done
			# fi

			#dodajanje starsev rekurzivno 
			addparent_recursive (){
				if [ $1 != 0 ]; then 
					tab+=($1)
					addparent_recursive "$(ps -o ppid= $1)"
					
				fi
			}
			addparent_recursive $x


			st=${#tab[@]}
			vre=0.6
			korak=$(bc <<< "scale=2; 6/$st")
			cnt=1
			cntele=$(( $st - 1))

			while [ $cntele -gt -1 ]; do
				pproc="${tab[cntele]}"
				#name="${proc%"("*}"
				name="$(cat /proc/$pproc/comm)"
				#pid="${proc#*"("}" && pid="${pid%")"*}"
				#SUDO - za odpiranje /proc/1/exe 
				path="$(readlink -s /proc/$pproc/exe)"
				nice=$(ps -o nice= -p $pproc)

				#BARVE
				if [[ $cnt == 1 || $cnt == $st ]]; then 
					[[ $cnt == 1 ]] && index=0
					[[ $cnt == $st ]] && index=5
				else
					vre=$(bc <<<  "scale=2; $vre+$korak")
					#echo "$vre"
					index=$(bc <<< "(($vre+0.5)/1)-1")
					#echo -e "${tab_m[$index]}123${reset}"
				fi
				cnt=$(( $cnt + 1 ))
				cntele=$(( cntele - 1 ))

				printf "${tab_m[$index]}$name $pproc $path\t $nice${reset}\n"
			done
			
			echo ""
			echo "[u] poskusi ubiti, [+] povišaj prijaznost, [-] znižaj prijaznost, [s] pošlji signal"
			beri () {
				read -s -n 1 ukaz
				case "$ukaz" in  
				    u)	
					kill -15 "$x"
					;;
				    +)
					newnice=$(( $cnice + 1 ))
				    renice -n "$newnice" -p "$x" >&/dev/null
					;;
					-)
					#SUDO - za znizevanje nice (prioritete) 
					newnice=$(( $cnice - 1 ))
				    renice -n "$newnice" -p "$x" >&/dev/null
					;;
					s)
					echo ""
					read -p "Vnesite oznako signala: " signal
					kill -"$signal" "$x"
					;;
					*)
					beri
					;;
				esac
			}
			beri

			if  ps -p $x &>/dev/null; then
				osvezi
			else
				clear
				echo "Proces je mrtev, ali pojdeš na pogreb?"
			fi

		else 
			printf "${red}Neveljaven PID${reset}\n"
		fi
	}
	osvezi

done