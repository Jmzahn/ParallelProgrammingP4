#!/bin/sh
#$ -V
#$ -cwd
#$ -S /bin/bash
#$ -N MatrixMul_2000
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


echo "Testing mpi_matrixmul.exe -np 1 ..."
mpirun --machinefile machinefile.$JOB_ID -np 1 ./mpi_matrixmul.exe 2000
echo -e "###\n"
echo "Testing mpi_matrixmul.exe -np 2 ..."
mpirun --machinefile machinefile.$JOB_ID -np 2 ./mpi_matrixmul.exe 2000
echo -e "###\n"
echo "Testing mpi_matrixmul.exe -np 4 ..."
mpirun --machinefile machinefile.$JOB_ID -np 4 ./mpi_matrixmul.exe 2000
echo -e "###\n"
echo "Testing mpi_matrixmul.exe -np 8 ..."
mpirun --machinefile machinefile.$JOB_ID -np 8 ./mpi_matrixmul.exe 2000
echo -e "###\n"
echo "Testing mpi_matrixmul.exe -np 16 ..."
mpirun --machinefile machinefile.$JOB_ID -np 16 ./mpi_matrixmul.exe 2000
echo -e "###\n"
echo "Testing mpi_matrixmul.exe -np 32 ..."
mpirun --machinefile machinefile.$JOB_ID -np 32 ./mpi_matrixmul.exe 2000
echo -e "###\n"
echo "Testing mpi_matrixmul.exe -np 64 ..."
mpirun --machinefile machinefile.$JOB_ID -np 64 ./mpi_matrixmul.exe 2000
echo -e "###\n"
