#!/bin/bash

# for i in 10 20 50 100 200 500 1000 2000 5000 10000
# do
#     ../../scripts/gradual_learn/learn.py ./child_nb.net ./child.csv $i ./learned_nets/child_nb.$i.net
# done

~/joey/opt/arg_ucla/ace/compile -noTabular -cd06 -dtBnMinfill ./$1.net

