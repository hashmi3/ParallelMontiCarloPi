#!/bin/bash
#
#SBATCH --cpus-per-task=4
#SBATCH --time=02:00
#SBATCH --mem=1G
#SBATCH --partition=fast

srun ./triangle_counting_parallel --nWorkers 4

#srun python /scratch/assignment1/test_scripts/submission_validator.pyc --tarPath=/home/muh/assig01_sub/assignment1.tar.gz
