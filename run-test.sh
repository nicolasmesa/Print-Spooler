#! /bin/bash

addqueue test_data/*
showqueue
rmqueue 1 2 3 4 5 6 7 8
rmqueue 1 2 3 4 5 6 7 8
addqueue test_data/file01
showqueue
rmqueue 1 2 3 4 5 6 7 8