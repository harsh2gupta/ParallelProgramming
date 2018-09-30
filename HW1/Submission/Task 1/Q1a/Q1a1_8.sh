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
#SBATCH -p skx-normal          # Queue (partition) name
#SBATCH -N 1               # Total # of nodes (must be 1 for serial)
#SBATCH -n 1               # Total # of mpi tasks (should be 1 for serial)
#SBATCH -t 00:30:00        # Run time (hh:mm:ss)

# Other commands must follow all #SBATCH directives...

# Launch serial code...

./Q1a1.out > Q1a1_out.txt
./Q1a2.out > Q1a2_out.txt
./Q1a3.out > Q1a3_out.txt
./Q1a4.out > Q1a4_out.txt
./Q1a5.out > Q1a5_out.txt
./Q1a6.out > Q1a6_out.txt
./Q1a7.out > Q1a7_out.txt
./Q1a8.out > Q1a8_out.txt
./Q1a9.out > Q1a9_out.txt
# ---------------------------------------------------
