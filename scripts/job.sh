#!/bin/bash

#SBATCH --nodes=1                      #Numero de Nós
#SBATCH --ntasks-per-node=1            #Numero de tarefas por Nó
#SBATCH --ntasks=1                     #Numero total de tarefas MPI
#SBATCH --cpus-per-task=24             #Numero de threads
#SBATCH -p cpu_dev                     #Fila (partition) a ser utilizada
#SBATCH -J fluid                       #Nome job
#SBATCH --exclusive                    #Utilização exclusiva dos nós durante a execução do job


#Exibe os nós alocados para o Job
echo "$SLURM_JOB_NODELIST"
nodeset -e "$SLURM_JOB_NODELIST"

cd "$SLURM_SUBMIT_DIR"

#Configura os compiladores
module load gcc/11.1

#Configura o executavel
BIN_NAME="fluid"
EXEC="$SCRATCH/$BIN_NAME"

#exibe informaçoes sobre o executavel
# /usr/bin/ldd "$EXEC"

#configura o numero de threads, de acordo com o parametro definido no Slurm
# export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

srun -N 1 -c "$SLURM_CPUS_PER_TASK" "$EXEC" -J CPU_FLUID -
