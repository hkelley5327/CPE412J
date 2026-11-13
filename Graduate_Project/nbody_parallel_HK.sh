#!/bin/bash

# Finish N=30000, T=1000 for remaining thread counts
for THREADS in 8 12 15
do
    export OMP_NUM_THREADS=$THREADS
    ./nbody_parallel_HK 30000 1000
done

# Run remaining particle counts
for N in 40000 50000
do
    for TIMESTEPS in 100 500 1000
    do
        for THREADS in 1 2 4 8 12 15
        do
            export OMP_NUM_THREADS=$THREADS
            ./nbody_parallel_HK $N $TIMESTEPS
        done
    done
done
