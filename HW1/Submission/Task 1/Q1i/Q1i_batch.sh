#!/bin/bash
#----------------------------------------------------
# Sample Slurm job script
#   for TACC Stampede2 KNL nodes
#
#   *** Serial Job on Normal Queue ***
# 
# Last revised: 20 Oct 2017
#
# Notes:
#
#   -- Copy/edit this script as desired.  Launch by executing
#      "sbatch knl.serial.slurm" on a Stampede2 login node.
#
#   -- Serial codes run on a single node (upper case N = 1).
#        A serial code ignores the value of lower case n,
#        but slurm needs a plausible value to schedule the job.
#
#   -- For a good way to run multiple serial executables at the
#        same time, execute "module load launcher" followed
#        by "module help launcher".

#----------------------------------------------------

#SBATCH -J myjob           # Job name
#SBATCH -p normal     # Queue (partition) name
#SBATCH -n 48              # Total # of mpi tasks (should be 1 for serial)
#SBATCH -t 00:30:00        # Run time (hh:mm:ss)
#SBATCH --mail-user=rmanhas@cs.stonybrook.edu
#SBATCH --mail-type=all    # Send email at begin and end of job

# Other commands must follow all #SBATCH directives...

module load papi
pwd
date

# Launch serial code...

./Q1iL1.o > Q1iL1.txt         # Do not use ibrun or any other MPI launcher
./Q1iL2.o > Q1iL2.txt
# ---------------------------------------------------
