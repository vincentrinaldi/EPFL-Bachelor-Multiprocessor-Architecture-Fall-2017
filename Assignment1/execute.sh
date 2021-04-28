#!/bin/bash
#SBATCH --workdir /scratch/<username>
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --reservation cs307cpu

echo STARTING AT `date`

./pi 16 5000000000

echo FINISHED at `date`