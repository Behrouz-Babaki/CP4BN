#!/usr/bin/python3

import sys
import struct

def getvvs(vv_f):
    line = vv_f.readline().strip()
    num_vars = int(line)
    vvs = []
    for i in range(num_vars):
        line = vv_f.readline()
        vv = int(vv_f.readline().strip())
        vvs.append(vv)
        for j in range(vv):
            vv_f.readline()
    return (num_vars, vvs)

def nextset(_attrset, _vvs):
    for i in reversed(range(len(_vvs))):
        if (_attrset[i] == _vvs[i]):
            if (i):
                _attrset[i] = 0
            else:
                return False
        else:
            _attrset[i] += 1
            break
    return True
        
def allProbs(attr_f_name, vv_f_name):
    with open(attr_f_name, 'rb') as attr_f:
        with open(vv_f_name, 'r') as vv_f:
            ipmap = dict()
            (numVars, _vvs) = getvvs(vv_f)
            _attrset = [0]*len(_vvs)
            hasnext = True
            while (hasnext):
                #print (_attrset,end='')
                p = attr_f.read(8)
                (prob,) = struct.unpack('>d', p)
                #print (' -> {}'.format(prob))
                ipmap[tuple(_attrset)] = prob
                hasnext = nextset(_attrset, _vvs)
    return (ipmap, numVars, _vvs)
                

def main():
    if len(sys.argv) != 3:
        print ("Usage: {} [attribute_file] [variable_value_file]".format(sys.argv[0]))
        exit(1)
    attr_f_name = sys.argv[1]
    vv_f_name = sys.argv[2]
    (ipmap, numVars, numValsList) = readFiles(attr_f_name, vv_f_name)
    for key in ipmap.keys():
        print ("{} --> {}".format(key, ipmap[key]))
    print ("number of variables: {}".format(numVars))
    print ("list of vlaue counts:")
    print (numValsList)


if __name__ == '__main__':
    main()
