#!/bin/bash

itemsetFile=$1
condesedType=$2
minConf=$3
outFile=$4
namesFile=$5

cmd="java -cp ./LimUtils/bin:./spmf.jar limutils.associationrules.ExtractRules ${itemsetFile} ${condesedType} ${minConf} ${outFile} ${namesFile}"
echo ${cmd}
${cmd}