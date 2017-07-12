#!/bin/bash
cd ../..
make
cd -
/usr/bin/time ../../bin/likely -t 0.0105  -a ./insurance.net.ac -l ./insurance.net.lmap --names ./insurance.names -v 
