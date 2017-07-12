#!/usr/bin/env python3
from __future__ import print_function
import sys

c_vals = {'cloud':2, 'sun':1}
r_vals = {'yes':2, 'no':1}
s_vals = {'on':2, 'off':1}
w_vals = {'yes':2, 'no':1}

def main():
    if len(sys.argv) < 2:
        print ('Usage: [csv file] [matlab-friendly file]')
        exit(1)
    csv_file = sys.argv[1]
    out_file = sys.argv[2]
    with open(csv_file, 'r') as c:
        line = c.readline().strip().split(',')
        #print(line)
        rows = []
        line = c.readline()
        while line != '':
            row = [0] * 4
            line = line.strip().split(',')
            row [0] = c_vals[line[0]]
            row [1] = s_vals[line[2]]
            row [2] = r_vals[line[1]]
            row [3] = w_vals[line[3]]
            row_str = [str(i) for i in row]
            rows.append(row_str)
            line = c.readline()

    with open(out_file, 'w') as w:
        for row in rows:
            print (','.join(row), file=w)

if __name__ == '__main__':
    main()
