#!/bin/bash

../../bin/likely -t 0.0105 -v -a ./insurance.net.ac -l ./insurance.net.lmap --names ./insurance.names  >  out.test 
#| tee  out.test
../interpret/interpret_cond2.py ./insurance.names ./out.test 1