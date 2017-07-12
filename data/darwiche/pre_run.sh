#!/bin/bash
cd ../..
make
cd -
/usr/bin/time ../../bin/likely -t 0.9  -a ./fs-04.net.ac -l ./fs-04.net.lmap --names ./fs-04.names -v  -c
