#!/bin/bash
for uporabnik in "${@:2}" 
do
     $ setfacl -m u:$uporabnik:wx $1
done

