#!/usr/bin/env python3

import itertools
import csv
import net_reader
import net_writer
import sys


def main():
    if len(sys.argv) < 4:
        print ('Usage: {} [network] [data] [nsamples] [learned net]'.format(sys.argv[0]))
        exit(1)
    
    net_file = sys.argv[1]
    data_file = sys.argv[2]
    nsamples = int(sys.argv[3])
    out_file = sys.argv[4]

    bn = net_reader.Bn(net_file)
    writer = net_writer.writer(bn)

    parent_counts = []
    node_counts = []
    for i in range(bn.num_nodes):
        current_node_counts = dict()
        p_nodes = [i] + bn.parents[i]
        p_names = [bn.node_values[j] for j in p_nodes]
        for vals in itertools.product(*p_names):
            current_node_counts[vals] = 0
        node_counts.append(current_node_counts)
        current_parents = dict()
        for vals in itertools.product(*p_names[1:]):
            current_parents[vals] = 0
        parent_counts.append(current_parents)

    data = csv.DictReader(open(data_file))
    row_counter = 1
    for row in data:
        if row_counter > nsamples:
            break
        for i in range(bn.num_nodes):
            p_nodes = [i] + bn.parents[i]
            p_names = [bn.node_names[j] for j in p_nodes]
            p_vals = [row[x] for x in p_names]
            node_counts[i][tuple(p_vals)] += 1
            parent_counts[i][tuple(p_vals[1:])] += 1
        row_counter += 1

    # print (node_counts)
    # print (parent_counts)

    # move this into the loop
    writer.write_net(node_counts, parent_counts, out_file)

if __name__ == '__main__':
    main()
