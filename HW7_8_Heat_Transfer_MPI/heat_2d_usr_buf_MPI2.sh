
#!/bin/bash
# Run serial version (if applicable)
./heat_2d_serial 8192 100 S

# Run MPI version with 1, 3, and 5 processes
mpiexec -n 1 ./heat_2d_usr_buf_MPI2 8192 100 S
mpiexec -n 3 ./heat_2d_usr_buf_MPI2 8192 100 S
mpiexec -n 5 ./heat_2d_usr_buf_MPI2 8192 100 S
