#!/bin/bash

../../bin/likely -t 0.4 -v -a ./mini.net.ac -l ./mini.net.lmap --names ./mini.names  >  out.test 
#| tee  out.test
./interpret_cond2.py ./mini.names ./out.test 1
