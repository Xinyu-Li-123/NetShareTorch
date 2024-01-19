#!/bin/bash
#SBATCH --job-name=NetShare-Caida
#SBATCH --partition=netsys
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --gres=gpu:1

SCRIPT_PATH="~/NetShareTorch/examples/ca"

mkdir -p "log"
module load anaconda3
source activate /gpfsnyu/home/xl3665/.conda/envs/NetShareTorch
cd $SCRIPT_PATH
{ time python driver.py; } > log/stdout.log 2> log/stderr.log
