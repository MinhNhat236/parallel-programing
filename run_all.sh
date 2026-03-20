#!/bin/bash

echo "Clearing previous timing file..."
> mpi_execution_times.txt

for np in 1 2 4 8
do
    echo "Running with $np processes"
    mpirun -np $np ./main
done