#! /bin/bash

echo "
---------------------------------
|	Tests started		|
---------------------------------

"

addqueue test_data/* test_data/inexistent-file 
showqueue
rmqueue 1 2 3 4 5 6 7 8
rmqueue 1 2 3 4 5 6 7 8
addqueue test_data/file01
showqueue
rmqueue 1 2 3 4 5 6 7 8

echo "
---------------------------------
|       Tests ended		|
---------------------------------

"
