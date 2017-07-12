#!/bin/bash
cd ../..
make
cd -
/usr/bin/time ../../bin/likely -t 0.0105  -a ./mini.net.ac -l ./mini.net.lmap --names ./mini.names -v 
