#!/bin/bash

inFile=$1
outFile=$2

cmd="java -cp ./LimUtils/bin:./inflib.jar limutils.convert.xml.net.ConvertXmlNet ${inFile} ${outFile}"
echo ${cmd}
${cmd}