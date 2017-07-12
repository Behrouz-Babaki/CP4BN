#!/usr/bin/python3

import readLikelyBin
import readLCMOutput
import sys

def overlap (l_output, f_output):
    (lipmap, threshold, numVars, numValsList) = readLikelyBin.likelyProbs (l_output);
    liSet = set(lipmap.keys())
    fiSet = readLCMOutput.frequentItemsets(f_output, numVars);
    
    intersection = len(liSet.intersection(fiSet))
    total = len(liSet) + len(fiSet) - intersection
    return (intersection,total)


def main():
    if len(sys.argv) != 3:
        print("Usage: {} [Likely_Sets_Binary_Output] [LCM_Output]".format(sys.argv[0]))
        exit(1)
        print (overlap (sys.argv[1], sys.argv[2]))
    
if __name__ == '__main__':
    main()
