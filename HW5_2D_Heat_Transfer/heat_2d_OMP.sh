#!/bin/bash
# 2D Heat Transfer: Serial Reference vs. OpenMP
module load gcc
set -e

# Compile both programs
g++ heat_2d_serial.cpp -o heat_2d_serial -std=c++11 -Ofast
g++ heat_2d_omp.cpp -o heat_2d_omp -fopenmp -std=c++11 -Ofast

# Serial reference: full matrix, execution time, and checksum
echo "=== Serial Reference: n=10, iterations=100 ===" > heat_2d_results.txt
./heat_2d_serial 10 100 >> heat_2d_results.txt

# OpenMP runs using 1, 2, 4, 8, and 16 threads
for threads in 1 2 4 8 16
do
    echo "" >> heat_2d_results.txt
    echo "=== OpenMP: n=10, iterations=100, threads=$threads ===" >> heat_2d_results.txt
    export OMP_NUM_THREADS=$threads
    ./heat_2d_OMP 10 100 >> heat_2d_results.txt
done

echo "All runs completed."
echo "Results saved in heat_2d_results.txt"

