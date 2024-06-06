#!/bin/bash
#SBATCH --job-name=TestNetShareTorch
#SBATCH --partition=netsys
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --gres=gpu:1

module load anaconda3
source activate /gpfsnyu/home/xl3665/.conda/envs/NetShareTorch
cd ~/NetShareTorch/examples/mytest
{ time python mytest.py; } >> ~/stdout.log 2>> ~/stderr.log
