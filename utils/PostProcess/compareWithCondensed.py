#!/usr/bin/python3

import sys
import readLikelyBin

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

def extractClosed(lipmap, domain_sizes):
    minDict = dict()
    maxDict = dict()
    for itemset in lipmap.keys():
        (minProb, maxProb) = lipmap[itemset]
        if minProb not in minDict.keys():
            minDict[minProb] = set()
        minDict[minProb].add(itemset)

    closed = set()
    for itemset1 in lipmap.keys():
        (minProb, maxProb) = lipmap[itemset1]
        fail = False
        for itemset2 in minDict[minProb]:
            if itemset2 != itemset1 and is_subset(itemset1, itemset2, domain_sizes):
                (minProb2,maxProb2) = lipmap[itemset2]
                if maxProb == maxProb2:
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
                #print ("is1: {} < {}".format(itemset2, itemset1))
                marked.add(itemset2)
            else:
                if is_subset(itemset1, itemset2, domain_sizes):
                    #print ("is1: {} < {}".format(itemset1, itemset2))
                    fail = True
                    break
        if not fail:
            for itemset in marked:
                maximal.remove(itemset)
            maximal.add(itemset1)

    return maximal

def main():
    if len(sys.argv) != 4:
        print("Usage: {} [Likely_Binary_Output] [Condensed_Binary_Output] [closed|maximal]".format(sys.argv[0]))
        exit(1)
    
    lFileName = sys.argv[1]
    cFileName = sys.argv[2]
    condType = sys.argv[3]
    if condType == 'closed':
        ctype = 1
    else: 
        if condType == 'maximal':
            ctype = 2
        else:
            print ("third argument should be either 'closed' or 'maximal'")
            exit(1)
    
    (l_lipmap, l_threshold, l_numVars, l_numValsList) = readLikelyBin.likelyProbs (lFileName)
    (c_lipmap, c_threshold, c_numVars, c_numValsList) = readLikelyBin.likelyProbs (cFileName)

    if l_threshold != c_threshold or l_numVars != c_numVars or l_numValsList != c_numValsList:
        print ("Two itemsets do not match")
        exit(1)

    cSet = set(c_lipmap.keys())
    if ctype == 1:
        c2Set = extractClosed(l_lipmap, l_numValsList)
    else:
        c2Set = extractMaximal(set(l_lipmap.keys()), l_numValsList)
    
    
    diffSet = cSet.symmetric_difference(c2Set)
    if (len(diffSet)):

        def print_items (itemset):
            print ("{", end='')
            for i in range(len(itemset)):
                if itemset[i] == l_numValsList[i]:
                    print ('-', end='')
                else:
                    print (itemset[i], end='')
                if i < len(itemset) - 1:
                    print (", ", end='')
            print("}")
            
        print ("CP PROBABLE SET:")
        for itemset in l_lipmap.keys():
            print_items (itemset)

        print ("CP CONDENSED SET:")
        for itemset in cSet:
            print_items (itemset)

        print ("COMPUTED CONDENSED SET:")
        for itemset in c2Set:
            print_items (itemset)

        print ("INTERSECTION SET:")

        for itemset in cSet.intersection(c2Set):
            print_items (itemset)

        print ("DIFFERENCE SET:")
        for itemset in diffSet:
            print_items (itemset)

    else:
        print ("SUCCESS!")


if __name__ == '__main__':
    main()
