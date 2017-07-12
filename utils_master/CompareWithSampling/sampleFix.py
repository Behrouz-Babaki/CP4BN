#!/usr/bin/python -u

import argparse
import compareLikelyLCM
import os

sampler = '/home/behrouz/Documents/KUL/test/gecode/likely_worlds/utils/CompareWithSampling/writeSamples.sh'

def main():
    parser = argparse.ArgumentParser (fromfile_prefix_chars='@')
    parser.add_argument ('-l', '--lcm-binary', help='LCM binary file', required=True)
    parser.add_argument ('-n', '--network', help='the Bayesian network in NET format', required=True)
    parser.add_argument ('-m', '--names-file', help='the names file generated by likely', required=True)
    parser.add_argument ('-b', '--itemset-binary', help='binary file containing itemsets', required=True)
    parser.add_argument ('-z', '--num_transactions', help='number of sampled transactions', required=True)
    parser.add_argument ('-d', '--database', help='path for writing database file', required=True)
    parser.add_argument ('-o', '--lcm_output', help = 'path for writing lcm output', required=True)
    parser.add_argument ('-t', '--threshold', help = 'probability threshold' , required=True)

    args = parser.parse_args()
    lcm_binary = args.lcm_binary
    network = args.network
    names_file = args.names_file
    itemset_binary = args.itemset_binary
    num_transactions = int(args.num_transactions)
    database = args.database
    lcm_output = args.lcm_output
    threshold = float(args.threshold)

    if os.path.isfile(database):
        os.remove (database)

    support = threshold * num_transactions
    # sample step transactions to database
    cmd = sampler + ' ' + network + ' ' + names_file + ' ' + database + ' ' + str(num_transactions)
    print (cmd)
    os.system(cmd)

    # run lcm > lcm_output
    cmd = '2>/dev/null ' + lcm_binary + ' F ' + database + ' ' + str(support) + ' ' + lcm_output + ' 1>&2'
    print (cmd)
    os.system(cmd)

    # compare and print 
    (intersection, total) = compareLikelyLCM.overlap (itemset_binary, lcm_output)
    ratio = float(intersection)/float(total)
    print ('ratio {}'.format(ratio))

    

if __name__ == '__main__':
    main()
