#!/usr/bin/python3
''' Performs a series of tests on a small set of networks and
thresholds '''

import os
import time
import sys
import argparse

DATA_DIR = '../data/small_nets'
BIN_FILE = '../bin/likely'
CMP_FILE = '../utils/CompareOutput/compareLikelyAll.py'
NETS_DIR = DATA_DIR + '/nets'
ENUM_DIR = DATA_DIR + '/enum'

BIN_FILE = BIN_FILE + ' -s '

CHK_PROB = False
CHK_CLOSED = False
CHK_MAXIMAL = False

def run_test(net_name, threshold, counter, out_dir):
    '''
    For a specified network and probability threshold, comparison
    tests are performed.  Variable 'counter' incrementally increases
    by caller to differentiate the output files.  'out_dir' is a
    directory where the intermediate files (containing itemsets) and
    detailed reports are stored.
    '''

    net_file = NETS_DIR + '/' + net_name + '.net'
    ac_file = net_file + '.ac'
    lmap_file = net_file + '.lmap'
    enum_file = ENUM_DIR + '/' + net_name + '.net.attr'
    varval_file = ENUM_DIR + '/' + net_name + '.net.varval'
    name_num = out_dir + '/' + net_name + '.' + str(counter)

    if CHK_PROB :
        # counting probabale itemsets
        cmd = BIN_FILE + ' -t ' + str(threshold) + ' -a ' + ac_file
        cmd += ' -l ' + lmap_file + ' -n ' + name_num + '.names '
        cmd += ' -b ' + name_num + '.bin'
        os.system(cmd)

        cmd = CMP_FILE + ' -b ' + name_num + '.bin'
        cmd += ' -n ' + name_num + '.names' + ' -a ' + enum_file
        cmd += ' -v ' + varval_file + ' -f ' + name_num + '.report'
        cmd += ' -t probable'
        os.system(cmd)
    
    if CHK_CLOSED :
        # counting closed probable itemsets
        cmd = BIN_FILE + ' -c -t ' + str(threshold)
        cmd += ' -a ' + ac_file + ' -l ' + lmap_file
        cmd += ' -n ' + name_num + '.names.c '
        cmd += ' -b ' + name_num + '.bin.c'
        os.system(cmd)

        cmd = CMP_FILE + ' -b ' + name_num + '.bin.c'
        cmd += ' -n ' + name_num + '.names.c'  + ' -a ' + enum_file
        cmd += ' -v ' + varval_file + ' -f ' + name_num + '.report.c'
        cmd += ' -t closed'
        os.system(cmd)

    if CHK_MAXIMAL :
        # counting maximal probabale itemsets
        cmd = BIN_FILE + ' -m -t ' + str(threshold)
        cmd += ' -a ' + ac_file + ' -l ' + lmap_file
        cmd += ' -n ' + name_num + '.names.m '
        cmd += ' -b ' + name_num + '.bin.m'
        os.system(cmd)

        cmd = CMP_FILE + ' -b ' + name_num + '.bin.m'
        cmd += ' -n ' + name_num + '.names.m'
        cmd += ' -a ' + enum_file
        cmd += ' -v ' + varval_file + ' -f ' + name_num + '.report.m'
        cmd += ' -t maximal'
        os.system(cmd)

def main():
    ''' main module '''
    parser = argparse.ArgumentParser()
    parser.add_argument("test_file",
                        type=str,
                        help="the file containing test cases")
    parser.add_argument("--probable", 
                        help="test generated probable itemsets",
                        action="store_true")
    parser.add_argument("--closed", 
                        help="test generated closed itemsets",
                        action="store_true")
    parser.add_argument("--maximal", 
                        help="test generated maximal itemsets",
                        action="store_true")
    args = parser.parse_args()

    global CHK_PROB
    global CHK_MAXIMAL
    global CHK_CLOSED
    CHK_PROB = args.probable
    CHK_MAXIMAL = args.maximal
    CHK_CLOSED = args.closed

    os.chdir(os.path.dirname(__file__))
    out_dir = '../test'
    out_dir += '/' + time.strftime('%d.%m-%H.%M')
    if os.path.exists(out_dir):
        out_dir = out_dir + time.strftime('.%S')
    os.mkdir(out_dir)
    cmd = 'git rev-parse HEAD > ' + out_dir + '/githash.txt'
    os.system(cmd)
    
    with open(args.test_file, 'r') as param_file:
        line = param_file.readline()
        while line:
            net_name = line.strip()
            print("file name: {}.net".format(net_name))
            line = param_file.readline().strip()
            counter = 1
            while line != '':
                threshold = float(line)
                print("threshold: {}".format(threshold))
                run_test(net_name, threshold, counter, out_dir)
                line = param_file.readline().strip()
                counter += 1
            line = param_file.readline().strip()

if __name__ == '__main__':
    main()
