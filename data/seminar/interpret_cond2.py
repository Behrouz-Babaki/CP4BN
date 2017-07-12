#!/usr/bin/env python3

import sys
import re
import operator

if len(sys.argv) < 2:
    print('Usage: {} [names file] [output file] [prob]'.format(sys.argv[0]))
    exit(1)

with open(sys.argv[1], 'r') as f:
    num_vars = int(f.readline().strip())
    varnames = [''] * num_vars
    varvals = [[]] * num_vars
    for i in range(num_vars):
        varnames[i] = f.readline().strip()
        numvals = int(f.readline().strip())
        vals = [''] * numvals
        for j in range(numvals):
            vals[j] = f.readline().strip()
        varvals[i] = vals

sol = []
with open(sys.argv[2], 'r') as f:
    for line in f:
        matchObj = re.match( r'([0-9]+)\$(1|(0\.[0-9]+)).*', line, re.M|re.I)
        if matchObj:
            num_part = matchObj.group(1)
            prob = matchObj.group(2)
            p = float(prob) / float(sys.argv[3])
            nums = [int(i) for i in list(num_part)]
            itemset =''
            for i in range(num_vars):
                if nums[i] < len(varvals[i]):
                    itemset +='{} = {}\n'.format(varnames[i], varvals[i][nums[i]])
            sol += [(itemset,p)]



sol2 = sorted(sol, key=operator.itemgetter(1), reverse=True)
for s in sol2:
    print (s[1])
    print (s[0])
    print
