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
#SBATCH -p development     # Queue (partition) name
#SBATCH -n 48              # Total # of mpi tasks (should be 1 for serial)
#SBATCH -t 00:05:00        # Run time (hh:mm:ss)
#SBATCH --mail-user=rmanhas@cs.stonybrook.edu
#SBATCH --mail-type=all    # Send email at begin and end of job
export CILK_NWORKERS=48
# Other commands must follow all #SBATCH directives...

module load papi

./Q1g10.out > Q1g_out.txt   

# ---------------------------------------------------
