#!/bin/bash

testroot="/tmp/OS-DN1"
mkdir -p "$testroot"
testskel="$testroot/skel"
resultsroot="$testroot"

testedscript="DN1a.sh"
results="$resultsroot/results-1a.log"
errlog="$resultsroot/err-1a.log"

source "print.sh"

prepare "$testedscript"
testedscript=./"$testedscript"

skel_dirs=(Desktop Documents Downloads Public)

# test
test -d "$testskel" && rez=0 || rez=1
assert_OK 1 $rez "predpogoji za test izpolnjeni ($testskel že obstaja):"
test $rez -eq 0 && { wrap_up; echo "Pobriši $testskel in ponovno poženi test."; exit 1; }

"$testedscript" "$testroot" >/dev/null 2> "$errlog" 

# test
test -d "$testskel" && rez=1 || rez=0
assert_OK 1 $rez "$testskel ustvarjen:"

# test
rez=1
for entry in ${skel_dirs[@]}; do
    test -d "$testskel/$entry" || { rez=0; break; }
done
assert_OK 1 $rez "dodatni imeniki v skel ustvarjeni:"

# test
rez=$(readlink "$testskel"/www-root)
assert_equal /var/www "$rez" "pravilna povezava www-root:"

# test
num=$(ls -a "$testskel" | wc -l)
num_skel=$(ls -a /etc/skel/ | wc -l)
assert_equal $((num_skel+5)) $num "pravilno število datotečnih zapisov v $testskel:"

# test
status=0
"$testedscript" a b >/dev/null 2>> "$errlog" || status=1
assert_OK 1 $status "pravi status pri odvečnih argumentih:"


wrap_up clean
