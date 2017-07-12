#!/bin/bash

# A script for testing 'compareWithCondensed.py'

Name=$1
threshold=$2

if [ "x$Name" == "x" ]; then
    Name=fs-01
fi
if [ "x$threshold" == "x" ]; then
    threshold=1
fi

base_dir="/home/behrouz/Documents/KUL/test/gecode/likely_worlds"
cmp="${base_dir}/utils/PostProcess/compareWithCondensed.py"
dName="${base_dir}/data/OutForTest/0_${threshold}/${Name}.net"

echo "comparing maximal ..."
cmd="${cmp}  ${dName}.binary ${dName}.binary.maximal maximal"
#echo ${cmd}
$cmd

echo "comparing closed ..."
cmd="${cmp}  ${dName}.binary ${dName}.binary.closed closed"
#echo ${cmd}
$cmd
