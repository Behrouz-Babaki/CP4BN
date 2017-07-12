#!/usr/bin/env python3

import sys
import re

if len(sys.argv) < 2:
    print('Usage: {} [names file]'.format(sys.argv[0]))
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

for i in range(num_vars):
    print ('{}  {}'.format(i, varnames[i]))
    for j in range(len(varvals[i])):
        print('\t{}\t{}'.format(j, varvals[i][j]))
    print ('')
