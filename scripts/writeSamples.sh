#!/bin/bash

netFile=$1
namesFile=$2
dbFile=$3
cases=$4

cmd="java -cp ./LimUtils/bin:./inflib.jar limutils.sample.TransactionWriter ${netFile} ${namesFile} ${dbFile} ${cases}"
echo ${cmd}
${cmd}