#!/bin/bash

N_VALUES=(100 250 500 750 1000 2500 5000 7500 10000 15000 20000 30000 40000 50000)
TIMESTEPS=(100 500 1000)

for N in "${N_VALUES[@]}"
do
    for T in "${TIMESTEPS[@]}"
    do
        ./nbody_serial $N $T
    done
done