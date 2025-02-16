#!/bin/bash
if [[ $# != 2 ]]; then 
	exit 21
fi
for d in $(find $1/* -type d); do
	mkdir -p ${d/$1/$2}
done
if [[ $? != 0 ]]; then
	exit 21
fi
ln -sf $(realpath $2) ~/škorenj

chmod -R o-rwx $2
chmod -R u-w $2
chmod g-rwx $2
chmod u-r $2
chmod +t $2
exit 0
