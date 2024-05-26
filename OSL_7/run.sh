#!/bin/bash

s_range=$(seq 0 2000)
for s_value in $s_range; do
    python2 relocation.py -s $s_value -c -n 100 > sample.txt
done
