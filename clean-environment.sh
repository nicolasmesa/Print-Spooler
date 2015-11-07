#! /bin/bash

DIRPATH="/home/user01/spooler"

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

rm -f /bin/addqueue /bin/rmqueue /bin/showqueue
rm -rf "$DIRPATH"

