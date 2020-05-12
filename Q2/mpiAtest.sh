#!/bin/sh
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N partA_test
#$ -o $JOB_NAME.o$JOB_ID
#$ -e $JOB_NAME.e$JOB_ID
#$ -q omni
#$ -pe mpi 72
#$ -l h_vmem=5.3G
#$ -l h_rt=00:20:00

# After 8 a.m. May 11th, 2020, you'll have to use #$ -P finals instead of #$ -P quanah
# to submit jobs for this class. Below I've changed the default setting to this setup.
#$ -P finals 

module purge
module load intel openmpi


echo "Testing partA.exe -np 1 ..."
mpirun --machinefile machinefile.$JOB_ID -np 1 ./partA.exe 4
echo -e "###\n"
echo "Testing partA.exe -np 2 ..."
mpirun --machinefile machinefile.$JOB_ID -np 2 ./partA.exe 4
echo -e "###\n"
echo "Testing partA.exe -np 4 ..."
mpirun --machinefile machinefile.$JOB_ID -np 4 ./partA.exe 4
echo -e "###\n"
echo "Testing partA.exe -np 8 ..."
mpirun --machinefile machinefile.$JOB_ID -np 8 ./partA.exe 8
echo -e "###\n"
echo "Testing partA.exe -np 16 ..."
mpirun --machinefile machinefile.$JOB_ID -np 16 ./partA.exe 16
echo -e "###\n"
echo "Testing partA.exe -np 32 ..."
mpirun --machinefile machinefile.$JOB_ID -np 32 ./partA.exe 32
echo -e "###\n"
echo "Testing partA.exe -np 64 ..."
mpirun --machinefile machinefile.$JOB_ID -np 64 ./partA.exe 64
echo -e "###\n"
