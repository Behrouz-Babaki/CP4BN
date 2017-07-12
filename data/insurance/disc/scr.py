#!/usr/bin/env python3

import re

with open('insurance.net', 'r') as f:
    for line in f:
        matchObj = re.match(r'node (.*)', line, re.M|re.I)
        if matchObj:
            print ('')
            print (matchObj.group(1))
            print (matchObj.group(1))
