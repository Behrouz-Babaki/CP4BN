#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
acfile="${DIR}/../data/small_nets/nets/asia.net.ac"
lmfile="${DIR}/../data/small_nets/nets/asia.net.lmap"
pairfile="${DIR}/../test/25.12-15.47/disc.txt"
bin="${DIR}/../bin/likely"

cmd="${bin} -d -a ${acfile} --acfile2 ${acfile} -l ${lmfile} --lmfile2 ${lmfile} -p ${pairfile} -t 0.51 -v"
$cmd
