#!/bin/bash
#SBATCH --workdir /path/to/working/directory
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 1
#SBATCH --mem 1G
#SBATCH --reservation cs307cpu

threads=1
length=10
iterations=1
output="output.csv"

./assignment2 $threads $length $iterations $output