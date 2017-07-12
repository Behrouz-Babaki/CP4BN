#!/bin/bash

netFile=$1
namesFile=$2
dbFile=$3
cases=$4
dir='/home/behrouz/Documents/KUL/test/gecode/likely_worlds/utils'

cmd="java -cp ${dir}/LimUtils/bin:${dir}/inflib.jar limutils.sample.TransactionWriter ${netFile} ${namesFile} ${dbFile} ${cases}"
#echo ${cmd}
${cmd}