#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void swap(int *arr, int i, int j)
{
    int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}

void q_sort(int *arr, int start, int end)
{
    int pivot, index;

    if (end <= 1)
        return;

    pivot = arr[start + end / 2];
    swap(arr, start, start + end / 2);

    index = start;

    for (int i = start + 1; i < start + end; i++)
    {
        if (arr[i] < pivot)
        {
            index++;
            swap(arr, i, index);
        }
    }

    swap(arr, start, index);

    q_sort(arr, start, index - start);
    q_sort(arr, index + 1, start + end - index - 1);
}

int *merge(int *arr1, int n1, int *arr2, int n2)
{
    int *result = (int *)malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;

    for (k = 0; k < n1 + n2; k++)
    {
        if (i >= n1)
        {
            result[k] = arr2[j];
            j++;
        }
        else if (j >= n2)
        {
            result[k] = arr1[i];
            i++;
        }

        else if (arr1[i] < arr2[j])
        {
            result[k] = arr1[i];
            i++;
        }

        else
        {
            result[k] = arr2[j];
            j++;
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    int number_of_elements;
    int *data = NULL;
    FILE *file = NULL;
    double time_taken;

    if (argc != 3)
    {
        printf("Não foram passados os 2 arquivos como argumento.\n");
        printf("São necessários os arquivos: input.txt e output.txt\n");
        exit(-1);
    }

    int number_of_process, rank_of_process;
    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
        printf("Erro no start da aplicação.n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);

    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Erro na abertura do arquivo\n");
        exit(-1);
    }

    fscanf(file, "%d", &number_of_elements);
    printf("\nQuantidade de elementos no arquivo: %d\n", number_of_elements);

    data = (int *)malloc(number_of_elements * sizeof(int));

    for (int i = 0; i < number_of_elements; i++)
        fscanf(file, "%d", &data[i]);

    // Impressão do arquivo
    printf("Elements in the array is : \n");
    for (int i = 0; i < number_of_elements; i++)
        printf("%d  ", data[i]);
    printf("\n");

    fclose(file);
    file = NULL;

    // Inicia o cronômetro
    time_taken -= MPI_Wtime();

    q_sort(data, 0, number_of_elements);

    // Pausa o cronômetro
    time_taken += MPI_Wtime();

    // Abre o segundo arquivo de saída (output.txt)
    file = fopen(argv[2], "w");

    if (file == NULL)
    {
        printf("Erro na abertura do arquivo\n");
        exit(-1);
    }

    fprintf(file, "\nQuantidade de elementos no arquivo: %d\n", number_of_elements);

    for (int i = 0; i < number_of_elements; i++)
        fprintf(file, "%d  ", data[i]);

    fclose(file);

    printf("\n\nResultado salvo no arquivo output.txt.\n");

    printf("Quantidade de elementos no arquivo: %d\n", number_of_elements);
    printf("Elementos do vetor após o sort: \n");

    for (int i = 0; i < number_of_elements; i++)
        printf("%d  ", data[i]);

    printf("\nQuick sorted %d ints.", number_of_elements);
    printf("\nDuração: %f secs\n", time_taken);
    free(data);

    MPI_Finalize();
    return 0;
}