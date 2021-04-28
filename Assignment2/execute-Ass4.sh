#!/bin/bash
#SBATCH --workdir /home/vrinaldi/Assignment2/
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 1
#SBATCH --mem 2G
#SBATCH --reservation cs307cpu

#length=10000
#iterations=500
#output="outputx.csv"

#./assignment2 1 $length $iterations $output
#./assignment2 2 $length $iterations $output
#./assignment2 4 $length $iterations $output
#./assignment2 8 $length $iterations $output
#./assignment2 16 $length $iterations $output

./assignment2 16 100 10 outputx-Ass4.csv
./assignment2 16 100 1000 outputx-Ass4.csv
./assignment2 16 1000 100 outputx-Ass4.csv
./assignment2 16 1000 10000 outputx-Ass4.csv
