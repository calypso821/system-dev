#!/bin/bash 
imenik="$(pwd)"
path="ab"
size=""
skupnav="0"
stevk="4"
t1="0"

if [ -d "$1" ]; then
	imenik="$1"
	[[ ! "$imenik" = /* ]] && path="re"
	shift
else
	[ ! ${1::1}  = "-" ] && 
	{ echo "imenik ki ste ga podali ne obstaja" >&2; exit 2; }
fi

while test $# -gt 0
do
    case "$1" in #preberemo $1 argument 
        -s)	
		size=$2 
		[[ ! "$size" =~ ^[0-9]+$ ]] && 
		{ echo "stikalo -s nima argumenta (ni numericno)" >&2; exit 2; }
		shift
		;;
        -t)	
		time="ON"
		[ ! -n "$2" ] || 
		[[ ! "$2" =~ ^[0-9]+$ ]] && 
		{ echo "stikalo -t nima argumenta (ni numericno)" >&2; exit 2; };
		t1=$(("$2" * 60 * 24))
		shift
		;;
		-d)
		stevk="$2"
		[[ ! "$stevk" =~ ^[0-9]+$ ]] && 
		{ echo "stikalo -d nima argumenta (ni numericno)" >&2; exit 2; }
		shift
		;;
		-o)
		izpis="ON"
		tip="$2"
		[[ ! "$tip" =~ ^[0-9]+$ ]] && [ ! "$tip" = "type" ] && 
		{ echo "stikalo -o nima argumenta (ni numericno ali type)" >&2; exit 2; }
		shift
		;;
		*)	
		echo "stikalo ne obstaja" >&2
		exit 2
    	;;
        
    esac
    shift #zamenjamo argument $2 z $1 (ce tega nebi bilo bi brali 1. argument do konca - neskonca zanka)
done

if [ -z "$size" ]; then 
	echo "niste podali obveznega stikala -s" >&2
	exit 2
fi

declare -a tabela
declare -a tabela2




mkdir out 2&>/dev/null


IFS=''
while read dat; do
	#Ce v podanem imeniku ni nobene datoteke vecje od podane velikosti
	[ "$dat" = "" ] && 
	{ echo "V imeniku ni datoteke te velikosti, podajte manjso velikost" >&2; exit 3; }


	name="$(basename $dat)"
	dir="$(dirname $dat)"
	[ "$path" = "re" ] &&  dir="$(pwd)/$dir"
	
	adate=$(stat -c "%X" "$dat")
	mdate=$(stat -c "%Y" "$dat")
	[ $adate -gt $mdate ] && maxdate=$adate  || maxdate=$mdate
	razlika=$(( $(date +%s) - $maxdate ))
	dni=$(( $razlika / (3600 * 24) ))
	velikost="$(($(stat -c "%s" "$dat") / 1024 / 1024))"

	#simbolicne povezave v imeniku out
	name1=$(printf "%0${stevk}d-${name}" $velikost)

	[ ! "$izpis" = "ON" ] && ln -s $dir out/$name1
	#sprotni izpis
	echo "$name $dir ($velikost MB) ($dni)" 

	tabela+=("$name $dir (*$velikost* MB) ($dni)")

	#sitaklo -o
	[ "$izpis" = "ON" ] &&
	if [ "$tip" = "type" ]; then 
		# Type 
		ime="${name##*.}"
		if [ "$ime" = "$name" ]; then #preveri ali vsebuje . ali ne
			ln -s "$dir" "out/$name1"
		else
			[ -d out/"$ime" ] && ln -s "$dir" "out/$ime/$name1" || 
			{ mkdir out/"$ime"; ln -s "$dir" "out/$ime/$name1"; }
			tabela2+=("$ime $velikost MB");
		fi
	else
		#d
		paket=$(( ((($velikost - $size) / $tip) * $tip) + $size ))
		[ -d out/"$paket" ] && ln -s "$dir" "out/$paket/$name1" || 
		{ mkdir out/"$paket"; ln -s "$dir" "out/$paket/$name1"; }
		tabela2+=("$paket $velikost MB")
	fi
	#koncna skupna vrednost v MB
	skupnav=$(($skupnav + $velikost))

done <<< "$(find "$imenik" -type f -size +"$size"M -amin +"$t1" -mmin +"$t1" 2>/dev/null)"

echo ---

#sortiran izpis

printf '%s\n' "${tabela[@]}" | sort -nr -t'*' -k2 | tr -d "*"

echo ---

#Izpis statistike za posamezni imenik 
[ "$izpis" = "ON" ] && 
printf '%s\n' "${tabela2[@]}" | awk '{ a[$1]+=$2 }END{ for(i in a) print i, a[i] " MB" }' | 
{ sort -nr -t' ' -k2; echo ---; }

echo "skupaj $skupnav MB"
