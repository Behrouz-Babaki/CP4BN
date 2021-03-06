#!/usr/bin/env python3

from __future__ import print_function
import net_reader
import itertools

class writer:
    def __init__(self, bn):
        self.bn = bn
        self.__potentials = []
        for i in range(bn.num_nodes):
            current_potential = dict()
            p_nodes = [i] + bn.parents[i]
            p_names = [bn.node_values[j] for j in p_nodes]
            for vals in itertools.product(*p_names):
                current_potential[vals] = 0
            self.__potentials.append(current_potential)
    
    def __compute(self, node_counts, parent_counts):
        for i in range(self.bn.num_nodes):
            p_nodes = [i] + self.bn.parents[i]
            p_names = [self.bn.node_values[j] for j in p_nodes]
            for vals in itertools.product(*p_names):
                pvals = tuple(list(vals)[1:])
                self.__potentials[i][vals] = (node_counts[i][vals]+1) / (parent_counts[i][pvals] + 
                                                                         len(self.bn.node_values[i]))

    def write_net(self, node_counts, parent_counts, filename):
        self.__compute(node_counts, parent_counts)
        buf = ''
        buf += self.header()
        buf += self.variables()
        buf += self.potentials()
        with open(filename, 'w') as f:
            print (buf, file=f)

    def header (self):
        buf = 'net\n{\n}\n\n'
        return buf

    def variables (self):
        buf = ''
        for i in range(self.bn.num_nodes):
            buf += 'node {}\n{{\n'.format(self.bn.node_names[i])
            buf += '\tstates = ('
            for j in range(len(self.bn.node_values[i])):
                buf += '"{}" '.format(self.bn.node_values[i][j])
            buf += ');\n}\n\n'
        return buf

    def potentials (self):
        buf = ''
        for i in range(self.bn.num_nodes):
            buf += 'potential ( ' + self.bn.node_names[i] + ' | '
            for p in self.bn.parents[i]:
                buf += self.bn.node_names[p] + ' '
            buf += ')\n{\n\tdata = \n'
            buf += self.table(i)
            buf += ';\n}\n\n'
        return buf
            
    def table (self, i):
        buf = ''
        buf += self.row (i, [])
        return buf

    def row (self, i, pvalnames):
        buf = ''
        if len(pvalnames) < len(self.bn.parents[i]):
            pid = self.bn.parents[i][len(pvalnames)]
            buf += '('
            for pvalname in self.bn.node_values[pid]:
                pvalnames.append(pvalname)
                #buf += 'parents[{} {}] --> '.format(pid, pvalname) +str(pvalname) + '\n'
                buf += self.row(i, pvalnames)
                pvalnames.pop()
            buf += ')\n'
        else:
            buf += '\n\t('
            for val in self.bn.node_values[i]:
                key = tuple([val] + pvalnames)
                p = self.__potentials[i][key]
                buf += str(p) + ' '
            buf += ')'
                
        return buf
            

            
        

def main():
    print ('Hello World!')

if __name__ == '__main__':
    main()
