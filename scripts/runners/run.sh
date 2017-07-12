#!/bin/bash

Name=$1
threshold=$2

base_dir="/home/behrouz/Documents/KUL/test/gecode/likely_worlds"

if [ "x$Name" == "x" ]; then
    Name=${base_dir}/compare/bin/fs-01.net
fi
if [ "x$threshold" == "x" ]; then
    threshold=0.1
fi


# compile the AC
if [[ -z "$ACE_HOME" ]]; then
    ACE_HOME="/cw/dtaijupiter/NoCsBack/dtai/behrouz/opt/arg_ucla/ace"
fi

${ACE_HOME}/compile -noTabular ${Name}


make 

# enumerate likely itemsets by CP model
cmd="${base_dir}/likely -t ${threshold} -a ${Name}.ac -l ${Name}.lmap -n ${Name}.names -b ${Name}.binary "
# echo ${cmd}
${cmd}

# enumerate closed likely itemsets by CP model
cmd="${base_dir}/likely -c -t ${threshold} -a ${Name}.ac -l ${Name}.lmap -n ${Name}.names -b ${Name}.binary.closed "
# echo ${cmd}
${cmd}

# enumerate maximal likely itemsets by CP model
cmd="${base_dir}/likely -m -t ${threshold} -a ${Name}.ac -l ${Name}.lmap -n ${Name}.names -b ${Name}.binary.maximal "
# echo ${cmd}
${cmd}


echo "extracting itemsets by brute-force method ..."
# enumerate all itemsets
cmd="java -cp ${base_dir}/attrset_enumerator/AceEval/bin:${base_dir}/attrset_enumerator/Enumerator/bin lim.enumerator.LIEnumerator ${Name}.ac ${Name}.lmap"
echo ${cmd}
${cmd}

echo "comparing results ..."
# compare results
cmd="${base_dir}/compare/py/compareLikelyAll.py  ${Name}.binary ${Name}.names ${Name}.attr ${Name}.varval"
echo ${cmd}
$cmd

echo "comparing closed ..."
cmd="${base_dir}/utils/PostProcess/compareWithCondensed.py ${Name}.binary ${Name}.binary.closed closed"
$cmd

echo "comparing maximal ..."
cmd="${base_dir}/utils/PostProcess/compareWithCondensed.py ${Name}.binary ${Name}.binary.maximal maximal"
$cmd
