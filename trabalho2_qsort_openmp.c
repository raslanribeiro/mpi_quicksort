#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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

    #pragma omp parallel for
    for (int i = start + 1; i < start + end; i++)
        {
            if (arr[i] < pivot)
            {
                index++;
                swap(arr, i, index);
            }
        }

    swap(arr, start, index);
    #pragma omp parallel sections
        {
        #pragma omp section
            q_sort(arr, start, index - start);
        #pragma omp section
            q_sort(arr, index + 1, start + end - index - 1);
        }
}

int main(int argc, char *argv[])
{   
    int num_of_threads;
    scanf("%d", &num_of_threads);
    omp_set_num_threads(num_of_threads);
    int number_of_elements;
    int *data = NULL;
    FILE *file = NULL;
    double start, end;
    int i;

    if (argc != 3)
    {
        printf("\nNão foram passados os 2 arquivos como argumento.\n");
        printf("São necessários os arquivos: input.txt e output.txt\n");
        exit(-1);
    }
    // Inicia o cronômetro
    start = omp_get_wtime();

    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Erro na abertura do arquivo\n");
        exit(-1);
    }

    fscanf(file, "%d", &number_of_elements);
    printf("\nQuantidade de elementos no arquivo de entrada: %d\n", number_of_elements);

    data = (int *)malloc(number_of_elements * sizeof(int));

    #pragma omp parallel for shared(data)
    // Assign de cada elemento no arquivo para o vetor data
    for (i = 0; i < number_of_elements; i++)
        fscanf(file, "%d", &data[i]);

    // Impressão de cada elemento do vetor
    printf("Elementos do vetor: \n");
    for (i = 0; i < number_of_elements; i++)
        printf("%d  ", data[i]);
    printf("\n");

    fclose(file);
    file = NULL;

    q_sort(data, 0, number_of_elements);

    file = fopen(argv[2], "w");

    if (file == NULL)
    {
        printf("Erro na abertura do arquivo.\n");
        exit(-1);
    }

    // Imprime o numero de elementos passados
    fprintf(file, "Quantidade de elementos no vetor: %d\n", number_of_elements);

    // Imprime cada elemento após o sort
    for (i = 0; i < number_of_elements; i++)
        fprintf(file, "%d  ", data[i]);

    fclose(file);

    printf("\n\nResultado salvo no arquivo output.txt.\n");

    // Impressão do resultado
    printf("Quantidade de elementos no arquivo de saída: %d\n", number_of_elements);
    printf("Elementos do vetor após o sort: \n");

    for (i = 0; i < number_of_elements; i++)
        printf("%d  ", data[i]);

    printf("\n\nQuick sorted %d ints.", number_of_elements);

    end = omp_get_wtime();
    printf("\nDuração: %f secs\n", end - start);
    return 0;
}