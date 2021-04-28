#!/bin/bash -l
#SBATCH --workdir /home/vrinaldi/Assignment4
#SBATCH --reservation cs307gpu
#SBATCH --time 01:00:00
#SBATCH --partition=gpu
#SBATCH --qos=gpu_free
#SBATCH --gres=gpu:1

module load gcc cuda

make

./assignment4 100 10
./assignment4 100 1000
./assignment4 1000 100
./assignment4 1000 10000
