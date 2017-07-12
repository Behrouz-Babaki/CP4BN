#!/usr/bin/env python3
import re

class Bn:
    def __init__(self, filename):
        self.__filename = filename
        self.read()
        self.num_nodes = len(self.node_names)
        
    def load_example(self):
        self.node_names = [ 'WetGrass',
                            'Cloudy',
                            'Sprinkler',
                            'Rain'];
        self.node_vals = { 'WetGrass': 0,
                           'Cloudy' : 1,
                           'Sprinkler' :2,
                           'Rain' : 3 };
        self.node_values = [['yes', 'no'],
                            ['cloud', 'sun'],
                            ['on', 'off'],
                            ['yes', 'no']];

        self.parents = [ [2,3],
                         [],
                         [1],
                         [1] ];
        return
        
    def read(self):
        self.node_names = []
        self.node_values = []
        self.node_vals = dict()
        with open(self.__filename, 'r') as f:
            line = f.readline()
            while line != '':
                matchObj = re.match( r'\s*node\s+(\w+)\s*.*', line, re.M|re.I)
                if matchObj:
                    node_name = matchObj.group(1)
                    self.node_vals [node_name] = len(self.node_names)
                    self.node_names.append(node_name)
                    line = f.readline()
                    matchObj = re.match( r'\s*states\s*=\s*\(([^\)]*)\).*', line, re.M|re.I)
                    while not matchObj:
                        line = f.readline()
                        matchObj = re.match( r'\s*states\s*=\s*\(([^\)]*)\).*', line, re.M|re.I)
                    states = matchObj.group(1)
                    states = states.strip().split()
                    names = [states[i][1:-1] for i in range(len(states))]
                    self.node_values.append(names)
                line = f.readline()

        # print (self.node_names)
        # print (self.node_values)
        # print (self.node_vals)
        
        self.parents = [[]] * len(self.node_names)
        with open(self.__filename, 'r') as f:
            line = f.readline()
            while line != '':
                matchObj = re.match( r'\s*potential\s*\(([\s\w]*)\|([\s\w]*)\)\.*', line, re.M|re.I)
                if matchObj:
                    child = matchObj.group(1)
                    parents = matchObj.group(2)
                    p_names = parents.strip().split()
                    self.parents[self.node_vals[child.strip()]] = [self.node_vals[p] for p in p_names]
                line = f.readline()
        # print (self.parents)
        return

def main():
    print

if __name__ == '__main__':
    main()
