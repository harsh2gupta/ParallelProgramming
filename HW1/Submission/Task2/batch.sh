#!/bin/bash
#----------------------------------------------------
#SBATCH -J myjob           # Job name
#SBATCH -p normal          # Queue (partition) name
#SBATCH -n 1              # Total # of mpi tasks
#SBATCH -t 00:05:00        # Run time (hh:mm:ss)
export CILK_NWORKERS=64
a.out
# ---------------------------------------------------
