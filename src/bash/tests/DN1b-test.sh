#!/bin/bash
testroot="/tmp/OS-DN1"
mkdir -p "$testroot"
testhome="$testroot/home"
testskel="$testroot/skel"
resultsroot="$testroot"

testedscript="DN1b.sh"
results="$resultsroot/results-1b.log"
errlog="$resultsroot/err-1b.log"

source "print.sh"

prepare "$testedscript" 
testedscript=./"$testedscript"


users=(u1 u3 u4)
nonusers=(u2)
skel_dirs=(Desktop Documents Downloads Public)

test_users=("${users[@]}" "${nonusers[@]}")

echo "Testni imenik: $testroot"

# test
rez=1
for usr in ${test_users[@]}; do
   id $usr &>/dev/null && rez=0
   test -d $testhome/$usr && rez=0
done
assert_OK 1 $rez "predpogoji za test izpolnjeni (testni uporabniki ne obstajajo):"
test $rez -eq 0 && { wrap_up; echo "Pobriši testne uporabnike (${test_users[*]}) in njihove domače imenike ter ponovno poženi test!"; exit 1; }

"$testedscript" users.conf "$testhome" "$testskel"  >/dev/null 2> "$errlog" 

# test
rez=1
for usr in ${users[@]}; do
    id $usr 1>/dev/null || rez=0
done
assert_OK 1 $rez "pravi uporabniki ustvarjeni (${users[*]}):"

# test
rez=1
for usr in ${nonusers[@]}; do
    id $usr 2>/dev/null && rez=0
done
assert_OK 1 $rez "pravi uporabniki izpuščeni (${nonusers[*]}):"

# test
rez=1
for usr in ${nonusers[@]}; do
    egrep "^${usr}$" "$errlog" 1>/dev/null || rez=0
done
assert_OK 1 $rez "izpuščeni uporabniki izpisani na standardni izhod za napake:"

# test
rez=1
for usr in ${users[@]}; do
    for entry in ${skel_dirs[@]}; do
        test -e "$testhome/$usr/$entry" || { rez=0; break; }
    done
    test $rez = 0 && break
done
assert_OK 1 $rez "dodatni imeniki iz skel v domačih imenikih:"

# test
rez=1
num=$(find $testskel | wc -l)
for usr in ${users[@]}; do
    test $num -eq "$(find $testhome/$usr | wc -l)" || rez=0
done
assert_OK 1 $rez "število dodatek v domačih imenikih enako imeniku skel:"

# test
rez=1
for usr in ${nonusers[@]}; do
    test -d $testhome/$usr && rez=0
done
assert_OK 1 $rez "domači imeniki izpuščenih uporabnikov ne obstajajo:"

# test
rez=1
for usr in ${users[@]}; do
    usrs=($(find $testhome/$usr | xargs -I {} stat -c "%U" {}))
    test ${#usrs[@]} -ge 1 || { rez=0; break; }
    for u in ${usrs[*]}; do
        test $u = $usr || { rez=0; break; }
    done
    test $rez = 0 && break
done
assert_OK 1 $rez "datoteke in imeniki imajo prave lastnike"

# test
rez=1
for usr in ${users[@]}; do
    grps=($(find $testhome/$usr | xargs -I {} stat -c "%G" {}))
    test ${#grps[@]} -ge 1 || { rez=0; break; }
    for g in ${grps[*]}; do
        test $g = $usr || { rez=0; break; }
    done
    test $rez = 0 && break
done
assert_OK 1 $rez "datoteke in imeniki imajo prave skupine"

wrap_up clean
