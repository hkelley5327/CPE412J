#!/bin/bash
mpiexec -n 5 ./heat_2d_loc_blk_MPI2_rev2 8192 100 | grep -E "Execution Time|Checksum"
