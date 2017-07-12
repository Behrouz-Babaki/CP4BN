#!/bin/bash
cd ../..
make
cd -
#ulimit -t 30
/usr/bin/time ../../bin/likely -t 0.8  -a ./insurance.net.ac -l ./insurance.net.lmap   -v # > ./out 2>&1
