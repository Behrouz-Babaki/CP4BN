#!/usr/bin/python3

import readLikelyBin
import readAllBin
import sys

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
    if len(sys.argv) != 5:
        print ("Usage: {} [Likely_Sets_Binary_Output] [name_file] [attribute_file] [variable_value_file]".format(sys.argv[0]))
        exit (1)
    (lipmap, threshold, numVars, numValsList)  = readLikelyBin.likelyProbs(sys.argv[1])
    (aipmap, numVars2, numValsList2) = readAllBin.allProbs(sys.argv[3], sys.argv[4])

    namesFileName = sys.argv[2]
    varvalFileName = sys.argv[4]

    (lVarNames, lValNames) = readVarVals (namesFileName)
    (aVarNames, aValNames) = readVarVals (varvalFileName)
    lVarNameToId = dict()
    lValNameToId = dict()
    for lVarNameCounter in range(len(lVarNames)):
        lVarNameToId [lVarNames[lVarNameCounter]] = lVarNameCounter
        for lValNameCounter in range(len(lValNames[lVarNameCounter])):
            lValNameToId [(lVarNames[lVarNameCounter], lValNames[lVarNameCounter][lValNameCounter])] = lValNameCounter
    
    def convertKey (key):
        key2 = [0] * len(aVarNames)
        for varId in range(len(aVarNames)):
            valId = key[varId] 
            varId2 = lVarNameToId [aVarNames[varId]]
            if valId < len(aValNames[varId]):
                valId2 = lValNameToId [(aVarNames[varId], aValNames[varId][valId])]
            else:
                valId2 = valId
            if valId == 0: hasZero = True
            key2 [varId2] = valId2
        return tuple(key2)

    # create another dictionary for likely itemsets from `all'
    lipmap2 = dict()
    for key in aipmap.keys():
        t = aipmap[key]
        hasZero = False
        if t >= threshold:
            key2 = convertKey(key)
            for attrCounter in range(len(key2)):
                if key2[attrCounter] == 0:
                    hasZero = True
            if not hasZero:
                lipmap2[key2] = t


    
    # Q1. Are all likely itemsets enumerated?
    diff1 = set(lipmap2.keys()).difference(set(lipmap.keys()))
    if len(diff1):
        print ("Itemsets found by ALL and missed by CP:")
        for key in diff1:
            print ("{} --> {}".format(key, lipmap2[key]))

    diff2 = set(lipmap.keys()).difference(set(lipmap2.keys()))
    if len(diff2):
        print ("Itemsets found by CP and missed by ALL:")
        for key in diff2:
            (minprob,maxprob) = lipmap[key]
            print ("{} --> [{} .. {}]".format(key, minprob, maxprob))

    
    # Q2. How close probabilities are?
    for key in set(lipmap.keys()).intersection(set(lipmap2.keys())):
        minprob = lipmap[key][0]
        maxprob = lipmap[key][1]
        trueprob = lipmap2[key]
        if (trueprob < minprob or trueprob > maxprob):
            print ("{} min:{} max:{} true:{}".format(key, minprob, maxprob, trueprob))


if __name__ == '__main__':
    main()
