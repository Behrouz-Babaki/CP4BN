#!/usr/bin/python3
import readLikelyBin
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

def main():
    parser = argparse.ArgumentParser(fromfile_prefix_chars='@')
    parser.add_argument("-b", "--binary-file", help="the binary output file of cp itemset miner", required=True)
    parser.add_argument("-n", "--names-file", help="the 'names' file produced by cp itemset miner", required=True)
    args = parser.parse_args()

    (lipmap, threshold, numVars, numValsList)  = readLikelyBin.likelyProbs(args.binary_file)
    namesFileName = args.names_file
    (lVarNames, lValNames) = readVarVals (namesFileName)
    domain_sizes = [len(val_names) for val_names in lValNames]

    for key in lipmap.keys():
        (min_prob, max_prob) = lipmap[key]
        print ('{',end='')
        first = True
        for i in range(len(key)):
            if key[i] != domain_sizes[i]:
                if first:
                    first = False
                else:
                    print (',',end='')
                print ('({}={})'.format(lVarNames[i], lValNames[i][key[i]]),end='')
        print ('}',end='')
        print (' -- {0:0.8f} -- {0:0.8f}'.format(min_prob, min_prob2))



if __name__ == '__main__':
    main()
