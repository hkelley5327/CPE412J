#!/bin/bash
# Run histogram_MPI with 1, 2, 4, and 5 MPI processes:
# 20 numbers, 4 bins, range 0-100, number list NOT suppressed.
# All output goes to histogram_results.txt (and still shows on screen).

OUTFILE=histogram_results.txt
: > "$OUTFILE"   # empty the file at the start of each run

for np in 1 2 3 4 5
do
   {
      echo "===== Running with $np MPI process(es) ====="
      mpiexec -np $np ./histogram_MPI_rev1 17 4 100.0 0.0
      echo
   } 2>&1 | tee -a "$OUTFILE"
done
