#!/bin/bash

Name=$1
threshold=$2

lmDir="/home/behrouz/Documents/KUL/test/gecode/likely_worlds"

# enumerate likely itemsets by CP model
cmd="${lmDir}/likely -t ${threshold} -a ${Name}.ac -l ${Name}.lmap -n ${Name}.names -b ${Name}.binary -v "
echo ${cmd}
${cmd}

