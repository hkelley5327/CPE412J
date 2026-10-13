#!/bin/bash
module load gcc
echo "=== Serial ===" > heat_2d_verify.txt
./heat_2d_serial 10 100 >> heat_2d_verify.txt 2>&1
for t in 1 2 4 8 16; do
  export OMP_NUM_THREADS=$t
  echo "=== OpenMP threads=$t ===" >> heat_2d_verify.txt
  ./heat_2d_OMP 10 100 >> heat_2d_verify.txt 2>&1
done
