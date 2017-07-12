#!/usr/bin/python3
from __future__ import print_function
import readLikelyBin
import readAllBin
import argparse


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

def is_subset(itemset1, itemset2, domain_sizes):
    if len(itemset1) != len(itemset2):
        print ("itemsets have different sizes!")
        exit(1)
    subset = True
    for varId in range(len(itemset1)):
        if itemset1[varId] != itemset2[varId] and itemset1[varId] != domain_sizes[varId]:
            subset = False
            break
    return subset

def extractClosed(lip2map, domain_sizes):
    probDict = dict()
    for itemset in lip2map.keys():
        prob = lip2map[itemset]
        if prob not in probDict.keys():
            probDict[prob] = set()
        probDict[prob].add(itemset)

    closed = set()
    for itemset1 in lip2map.keys():
        prob = lip2map[itemset1]
        fail = False
        for itemset2 in probDict[prob]:
            if itemset2 != itemset1 and is_subset(itemset1, itemset2, domain_sizes):
                prob2 = lip2map[itemset2]
                if prob == prob2:
                    fail = True
                    break
        if not fail:
            closed.add(itemset1)

    return closed

def extractMaximal(itemsets, domain_sizes):
    maximal = set()
    marked = set()
    while itemsets:
        marked.clear()
        itemset1 = itemsets.pop()
        fail = False
        for itemset2 in maximal:
            if is_subset(itemset2, itemset1, domain_sizes):
                marked.add(itemset2)
            else:
                if is_subset(itemset1, itemset2, domain_sizes):
                    fail = True
                    break
        if not fail:
            for itemset in marked:
                maximal.remove(itemset)
            maximal.add(itemset1)

    return maximal


def main():
    parser = argparse.ArgumentParser(fromfile_prefix_chars='@')
    parser.add_argument("-b", "--cp-binary", help="the binary output file of cp itemset miner", required=True)
    parser.add_argument("-a", "--enum-binary", help="the binary output of brute-force itemset enumerator", required=True)
    parser.add_argument("-n", "--names-file", help="the 'names' file produced by cp itemset miner", required=True)
    parser.add_argument("-v", "--varval-file", help="the 'varval' file produced by brute-force itemset enumerator", required=True)
    parser.add_argument("-t", "--itemset-type", help="itemset types (probable, maximal, or closed)", type=str, default='probable')
    parser.add_argument("-f", "--report-file", help="a file to which a detailed report will be written")
    args = parser.parse_args()

    if args.itemset_type not in {'probable', 'closed', 'maximal'}:
        print ("Itemset type should be 'probable', 'maximal', or 'closed'")
        exit(1)

    rpf = None
    if (args.report_file != None):
        rpf = open(args.report_file, 'w')
    

    (lipmap, threshold, numVars, numValsList)  = readLikelyBin.likelyProbs(args.cp_binary)
    (aipmap, numVars2, numValsList2) = readAllBin.allProbs(args.enum_binary, args.varval_file)

    namesFileName = args.names_file
    varvalFileName = args.varval_file

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
            key2 [varId2] = valId2
        return tuple(key2)

    # create another dictionary for likely itemsets from `all'
    lipmap2 = dict()
    for key in aipmap.keys():
        t = aipmap[key]
        if t >= threshold:
            key2 = convertKey(key)
            lipmap2[key2] = t


    lipkeys = lipmap.keys()
    lip2keys = lipmap2.keys()

    domain_sizes = [len(val_names) for val_names in lValNames]
    if args.itemset_type == 'closed':
        lip2keys = extractClosed (lipmap2, domain_sizes)
    elif args.itemset_type == 'maximal':
        lip2keys = extractMaximal (set(lip2keys), domain_sizes)

    # Q1. Are all likely itemsets enumerated?
    diff1 = set(lip2keys).difference(set(lipkeys))
    if len(diff1) and args.report_file != None:
        print ("Itemsets found by ALL and missed by CP:", file=rpf)
        for key in diff1:
            print ("{} --> {}".format(key, lipmap2[key]), file=rpf)

    diff2 = set(lipkeys).difference(set(lip2keys))
    if len(diff2) and args.report_file != None:
        print ("Itemsets found by CP and missed by ALL:", file=rpf)
        for key in diff2:
            (minprob,maxprob) = lipmap[key]
            print ("{} --> [{} .. {}]".format(key, minprob, maxprob), file=rpf)

    intersect = set(lipkeys).intersection(set(lip2keys))

    if (len(diff1) == 0 and len(diff2) == 0):
        print ("[OK]\tEquivalent sets of itemsets")
    else:
        print ("[FAIL]\tCP:{} Enum:{} intersection:{}".format(len(lipkeys), len(lip2keys), len(intersect)))
    
    # Q2. How close probabilities are?
    out_count = 0
    first = True
    for key in intersect:
        minprob = lipmap[key][0]
        maxprob = lipmap[key][1]
        trueprob = lipmap2[key]
        if (trueprob < minprob or trueprob > maxprob):
            if (first and args.report_file != None):
                print ("Itemsets with different probabilities frome CP than brute-force enumeration", file=rpf)
                first = False
            out_count += 1
            if (args.report_file != None):
                print ("{} min:{} max:{} true:{}".format(key, minprob, maxprob, trueprob), file=rpf)
    if out_count == 0:
        print ("[OK]\tAll probabilities in intersection ({}) within range".format(len(intersect)))
    else:
        print ("[FAIL]\tintersection:{} in_range:{}".format(len(intersect), len(intersect)-out_count))

    if (args.report_file != None):
        rpf.close()

if __name__ == '__main__':
    main()
