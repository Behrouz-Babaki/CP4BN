#!/bin/bash

Name=$1
threshold=$2

lmDir="/home/behrouz/Documents/KUL/test/gecode/likely_worlds"

# compile the AC
~/joey/opt/arg_ucla/ace/compile -noTabular ${Name}

# enumerate likely itemsets by CP model
cmd="${lmDir}/likely -t ${threshold} -a ${Name}.ac -l ${Name}.lmap -n ${Name}.names -b ${Name}.binary"
echo ${cmd}
${cmd}

# enumerate all itemsets
java -cp ${lmDir}/attrset_enumerator/AceEval/bin:${lmDir}/attrset_enumerator/Enumerator/bin lim.enumerator.LIEnumerator ${Name}.ac ${Name}.lmap

# compare results
${lmDir}/compare/py/compareLikelyAll.py  ${Name}.binary ${Name}.names ${Name}.attr ${Name}.varval 