#!/bin/bash

/usr/bin/time ../../../bin/likely -d --acfile ./adolescent.net.ac --lmfile ./adolescent.net.lmap --acfile2 ./senior.net.ac --lmfile2 ./senior.net.lmap -v -t 0 -p pair --names adolescent.names

# /usr/bin/time ../../../bin/likely -d --acfile ./insurance.net.ac --lmfile ./insurance.net.lmap --acfile2 ./nb.net.ac --lmfile2 ./nb.net.lmap --names ./nb_insurance.names --pair ./nb_pair -v -t 0.25
