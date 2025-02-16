#!/bin/bash
if [ "$#" -ne 1 ]; then 
echo "Error: podali ste vec kot 1 argument"
exit 1
else
mkdir -p $1 
mkdir $1/skel
cp -r /etc/skel/.	$1/skel
mkdir $1/skel/Desktop
mkdir $1/skel/Documents
mkdir $1/skel/Downloads
mkdir $1/skel/Public
mkdir /var/www
ln -s /var/www $1/skel/www-root
fi
exit 0


