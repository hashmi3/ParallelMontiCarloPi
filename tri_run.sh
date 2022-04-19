#!/bin/bash

rm slurm-*
#make triangle_counting
make triangle_counting_parallel
sbatch submit_triangle.sh
squeue
echo "################################"
sleep 2 && less sl*

