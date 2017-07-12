#!/bin/bash

# A script for testing 'compareLikelyAll.py'

Name=$1
threshold=$2

if [ "x$Name" == "x" ]; then
    Name=fs-01
fi
if [ "x$threshold" == "x" ]; then
    threshold=1
fi

base_dir="/home/behrouz/Documents/KUL/test/gecode/likely_worlds"
cmp="${base_dir}/utils/CompareOutput/compareLikelyAll.py"
dName="${base_dir}/data/OutForTest/0_${threshold}/${Name}.net"

echo "comparing probable ..."
cmd="${cmp}  -b ${dName}.binary -n ${dName}.names -a ${dName}.attr -v ${dName}.varval -f /home/behrouz/Desktop/1 -t probable"
#echo ${cmd}
$cmd

echo "comparing maximal ..."
cmd="${cmp}  -b ${dName}.binary.maximal -n ${dName}.names -a ${dName}.attr -v ${dName}.varval -f /home/behrouz/Desktop/2 -t maximal"
#echo ${cmd}
$cmd

echo "comparing closed ..."
cmd="${cmp}  -b ${dName}.binary.closed -n ${dName}.names -a ${dName}.attr -v ${dName}.varval -f /home/behrouz/Desktop/3 -t closed"
#echo ${cmd}
$cmd
