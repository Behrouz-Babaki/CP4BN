#!/usr/bin/python

import sys

def frequentItemsets (outFileName, numVars):
    with open(outFileName, 'r') as outFile:
        itemsets = set()
        for line in outFile:
            items = [int(i) for i in line.split()]
            itemset = [-1] * numVars
            for var in range(numVars):
                if var in items:
                    itemset[var] = 1
                else:
                    itemset[var] = 2
            itemsets.add(tuple(itemset))
    return itemsets

def main():
    if len(sys.argv) != 3:
        print ("Usage: {} [LCM_Output] [#items]".format(sys.argv[0]))
        exit (1)
    numVars = int (sys.argv[2])
    fiList  = frequentItemsets(sys.argv[1], numVars)
    print ("number of items: {}".format(numVars))
    for itemset in fiList:
        print ("{}".format(itemset))

if __name__ == '__main__':
    main()
