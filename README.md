$ mpicc -o exercicio exercicio1.c
$ mpirun -n 6 teste
$ mpirun -n 12 --use-hwthread-cpus exercicio
$ mpirun -n 12 --oversubscribe exercicio
