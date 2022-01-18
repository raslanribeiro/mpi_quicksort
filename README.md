# About

Algoritmos de quick sort paralelo e sequencial

## Commands

```bash
$ mpicc -o exercicio exercicio1.c
$ mpirun -n 6 teste
$ mpirun -n 12 --use-hwthread-cpus exercicio
$ mpirun -n 12 --oversubscribe exercicio
```

## Commands openmp

```bash
$ gcc -o trabalho2_qsort_openmp -fopenmp trabalho2_qsort_openmp.c
$ ./trabalho2_qsort_openmp data/input_200000_elements.txt data/output_200000_elements.txt
```
