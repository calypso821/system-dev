#!/bin/bash
diff <(tail -n+2 $1 | sort -t"," -k4.1,4.2n -k5n | sort -t"." -k2n -s | awk -F "," '{print $2}') <(tail -n+2 $2 | sort -t"," -k4.1,4.2n -k5n | sort -t"." -k2n -s | awk -F "," '{print $2}') &> /dev/null
#tail odstrani zacetno vrstico (head)
#uporabimo sort -t razdeimo med stolpce ".", razvrstimo po mesecu -k2n -s (stabilize sort)
#uporabimo ponovno sort -t razdeimo med stolpce ",", razvrstimo po dnevu (-k4.1,4.2n) in uri -k5n
#awk -F "," razdeli na stolpce med "," in izbere 2 stolpec za primerjanje 