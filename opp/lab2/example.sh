#!/bin/sh
#PBS -l walltime=01:00:00
#PBS -l select=1:ncpus=12:ompthreads=12

cd $PBS_O_WORKDIR
echo "OMP_NUM_THREADS = $OMP_NUM_THREADS\n"

gcc example.c -std=c99 -fopenmp -lm -O0 -o example

OMP_NUM_THREADS=12 ./example
