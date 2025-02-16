#!/bin/bash

testroot="/tmp/OS-DN1"
mkdir -p "$testroot"
resultsroot="$testroot"

testedscript="DN1c.sh"
results="$resultsroot/results-1c.log"
errlog="$resultsroot/err-1c.log"

source "print.sh"

prepare "$testedscript"
testedscript=./"$testedscript"

users=(u1 u3)
rm -f $testroot/testfile.txt &>/dev/null
echo "some content" > $testroot/testfile.txt

"$testedscript" "$testroot/testfile.txt" ${users[*]}

rez=1
for usr in ${users[@]}; do
   id $usr &>/dev/null || rez=0
done
assert_OK 1 $rez "predpogoji za test izpolnjeni (testni uporabniki obstajajo):"
test $rez -eq 0 && { wrap_up; echo "Ustvari testne uporabnike (${users[*]})!"; exit 1; }

# test
for usr in ${users[@]}; do
    rez=1
    echo $(getfacl $testroot/testfile.txt 2>/dev/null) | grep "user:$usr:r-x" >/dev/null || rez=0
    assert_OK 1 $rez "pravilne pravice za $usr:"
done



wrap_up clean
