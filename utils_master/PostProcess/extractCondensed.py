#!/usr/bin/python3

import sys
import readLikelyBin

def is_subset(itemset1, itemset2):
    if len(itemset1) != len(itemset2):
        print ("itemsets have different sizes!")
        exit(1)
    subset = True
    for varId in range(len(itemset1)):
        if itemset1[varId] < itemset2[varId]:
            subset = False
            break
    return subset

def extractClosed(lipmap):
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
            if itemset2 != itemset1 and is_subset(itemset1, itemset2):
                (minProb2,maxProb2) = lipmap[itemset2]
                if maxProb == maxProb2:
                    fail = True
                    break
        if not fail:
            closed.add(itemset1)

    return closed

def extractMaximal(itemsets):
    maximal = set()
    marked = set()
    while itemsets:
        marked.clear()
        itemset1 = itemsets.pop()
        fail = False
        for itemset2 in maximal:
            if is_subset(itemset2, itemset1):
                marked.add(itemset2)
            else:
                if is_subset(itemset1, itemset2):
                    fail = True
                    break
        if not fail:
            for itemset in marked:
                maximal.remove(itemset)
            maximal.add(itemset1)

    return maximal

def main():
    if len(sys.argv) != 4:
        print("Usage: {} [Likely_Binary_Output] [Condensed_Sparse_Output] [closed|maximal]".format(sys.argv[0]))
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

    if ctype == 1:
        c2Set = extractClosed(l_lipmap)
    else:
        c2Set = extractMaximal(set(l_lipmap.keys()))

    def write_itemset (out_file, itemset):
        size = len(itemset)
        for i in range(size):
            if itemset[i] is 1:
                c_file.write(str(i))
                if i < size - 1:
                    out_file.write (' ')
        out_file.write('\n')
        (min_prob, max_prob) = l_lipmap[itemset]
        out_file.write(str(min_prob) + '\n')
        out_file.write(str(max_prob) + '\n')


    with open (cFileName, 'w') as c_file:
        c_file.write(str(l_threshold) + '\n')
        c_file.write(str(l_numVars) + '\n')
        for num_vals in l_numValsList:
            c_file.write(str(num_vals) + '\n')
        for itemset in c2Set:
            write_itemset (c_file, itemset)

if __name__ == '__main__':
    main()
