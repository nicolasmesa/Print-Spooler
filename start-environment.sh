#! /bin/bash

DIRPATH="/home/user01/spooler"
USERGROUP="user01:user01"

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


echo "Setting up permissions"

cp addqueue /bin/addqueue
cp rmqueue /bin/rmqueue
cp showqueue /bin/showqueue

chown "$USERGROUP" /bin/addqueue /bin/rmqueue /bin/showqueue
chmod 111 /bin/addqueue /bin/rmqueue /bin/showqueue
chmod +s /bin/addqueue /bin/rmqueue /bin/showqueue

echo "Creating directories"


rm -rf "$DIRPATH"

umask 077
mkdir -p "$DIRPATH/config"
mkdir -p "$DIRPATH/printer"

echo "Setting directory permissions"

chown -R "$USERGROUP" "$DIRPATH"

# No need but just in case
chmod 700 -R "$DIRPATH"

echo "Done"
