#!/bin/bash

Name=$1
threshold=$2

lmDir="/home/behrouz/Documents/KUL/test/gecode/likely_worlds"

# compile the AC
~/jupiter/opt/arg_ucla/ace/compile -noTabular -cd05 -dtBnMinfill ${Name}

# enumerate likely itemsets by CP model
cmd="${lmDir}/likely -t ${threshold} -a ${Name}.ac -l ${Name}.lmap -n ${Name}.names -b ${Name}.binary -v "
echo ${cmd}
${cmd}

