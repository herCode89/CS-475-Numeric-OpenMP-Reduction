#!/bin/bash



# number of threads:
for t in 1 2 4 6 8 
	do

# number of subdivisions:
for s in 2 4 6 8 32 64 256 1024 2048 
do
    g++ -DNUMNODES=$s -DNUMT=$t proj2.cpp -o proj2 -lm -fopenmp
    ./proj2
  done
done