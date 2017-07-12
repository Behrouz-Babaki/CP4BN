#!/bin/bash
cd ../..
make
cd -
ulimit -t 3600
/usr/bin/time ../../bin/likely -t $2  -a ./$1.net.ac -l ./$1.net.lmap  > ./outputs/$1.$2.out 2>&1
