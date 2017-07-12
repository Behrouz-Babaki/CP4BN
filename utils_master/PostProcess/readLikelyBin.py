#!/usr/bin/python3

import struct
import sys

def bits(b):
    bts = []
    for i in range(8):
        bts.append((b>>i) & 1)
    bts = [b for b in reversed(bts)]
    return bts

def bl2int(bl):
    res = 0
    for b in bl:
        res = (res << 1) | b
    return res

def numBits(b):
    if not b:
        return 0
    h = 0
    for i in range(64):
        if ((b>>i)&1):
            h = i
    return h+1

def bitSlices(bl,nv):
    res = []
    byteList = []
    for b in bl:
        byteList += b
    end = len(byteList)
    for i in reversed(nv):
        start = end - i;
        res.append(byteList[start:end])
        end = start
    return [r for r in reversed(res)]

def likelyProbs(binaryFileName):
    with open( binaryFileName, 'rb') as f:
        s = f.read(16);
        (threshold, numVars) = struct.unpack('<dQ', s)
        nv = []
        numValsList = []
        totalBits = 0
        for i in range(numVars):
            s = f.read(8)
            (numVals,) = struct.unpack('<Q', s)
            numValsList.append(numVals)
            nv.append(numBits(numVals))
            totalBits += numBits(numVals)
        if totalBits%8 != 0:
            totalBits += 8-(totalBits%8)
        ipmap = dict()
        s = f.read(1)
        while s != b'':
            (b,) = struct.unpack('<B',s)
            itembytes = [bits(b)]
            for k in range(int(totalBits/8)-1):
                s = f.read(1)
                (b,) = struct.unpack('<B',s)
                itembytes.append(bits(b))
            t = ()
            for bs in bitSlices(itembytes,nv):
                t += (bl2int(bs),)
            s = f.read(16)
            (minprob,maxprob) = struct.unpack('<dd', s)
            ipmap[t] = (minprob,maxprob)
            s = f.read(1)
    return (ipmap, threshold, numVars, numValsList)

def main():
    if len(sys.argv) != 2:
        print ("Usage: {} [Likely_Sets_Binary_Output]".format(sys.argv[0]))
        exit (1)
    (ipmap, threshold, numVars, numValsList)  = likelyProbs(sys.argv[1])
    for key in ipmap.keys():
        t = ipmap[key]
        print ("{} --> [{}..{}]".format(key, t[0], t[1]))
    print ("threshold: {}".format(threshold))
    print ("number of variables: {}".format(numVars))
    print ("list of value counts:")
    print (numValsList)

if __name__ == '__main__':
    main()

