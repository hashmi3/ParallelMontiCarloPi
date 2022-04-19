#!/bin/bash

rm slurm-*
make pi_calculation
sbatch submit_pi.sh
squeue
echo "################################"
sleep 2 && less sl*
