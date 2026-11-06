#!/bin/bash
mpiexec -n 1 ./heat_2d_loc_blk_MPI2_rev1 4096 100 | grep -E "Execution Time|Checksum"
