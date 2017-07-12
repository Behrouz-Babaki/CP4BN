#!/usr/bin/python

import readLikelyBin
import sys
import operator

def readVarVals (fileName):
    with open(fileName) as namesFile:
        numVars = int(namesFile.readline())
        varNames = []
        valNames = []
        for varCounter in range(numVars):
            varName = namesFile.readline().strip()
            varNames.append(varName)
            numVals = int(namesFile.readline())
            currentValNames = []
            for valCounter in range(numVals):
                valName = namesFile.readline().strip()
                currentValNames.append(valName)
            valNames.append(currentValNames)
        return (varNames, valNames)

def main():
    if len(sys.argv) != 3:
        print ("Usage: {} [Likely_Sets_Binary_Output] [name_file] ".format(sys.argv[0]))
        exit (1)
    (lipmap, threshold, numVars, numValsList)  = readLikelyBin.likelyProbs(sys.argv[1])
    liplist = sorted(lipmap.iteritems(), key=operator.itemgetter(1), reverse = True)
    (lVarNames, lValNames) = readVarVals (sys.argv[2])


    for (key, prob) in liplist:
        print "{", 
        firstvar = True
        for varId in range(len(key)):
            valId = key[varId]
            if valId < len(lValNames [varId]):
                varName = lVarNames [varId]
                valName = lValNames [varId][valId]
                if firstvar:
                    print "{}={}".format(varName, valName), 
                    firstvar = False
                else:
                    print ", {}={}".format(varName, valName), 
        print "}} {}".format(prob)

if __name__ == '__main__':
    main()
