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
#SBATCH -n 68              # Total # of mpi tasks (should be 1 for serial)
#SBATCH -t 01:30:00        # Run time (hh:mm:ss)

# Other commands must follow all #SBATCH directives...

module load papi

# Launch serial code...

#./Q1h4.out > Q1h4_out.txt
#./Q1h5.out > Q1h5_out.txt
#./Q1h6.out > Q1h6_out.txt
#./Q1h7.out > Q1h7_out.txt 
#./Q1h8.out > Q1h8_out.txt
#./Q1h9.out > Q1h9_out.txt
./Q1h10.out > Q1h10_out.txt
./Q1h11.out > Q1h11_out.txt
./Q1h12.out > Q1h12_out.txt
./Q1h13.out > Q1h13_out.txt
#./Q1h14.out > Q1h14_out.txt
#./Q1h15.out > Q1h15_out.txt
 
# ---------------------------------------------------
