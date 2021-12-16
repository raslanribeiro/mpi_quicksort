#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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
    int chunk_size, self_chunk_size;
    int *chunk;
    FILE *file = NULL;
    double start, end;
    MPI_Status status;

    if (argc != 3)
    {
        printf("\nNão foram passados os 2 arquivos como argumento.\n");
        printf("São necessários os arquivos: input.txt e output.txt\n");
        exit(-1);
    }

    int process_number, rank_of_process;
    int rc = MPI_Init(&argc, &argv);
    // Inicia o cronômetro
    start = MPI_Wtime();

    if (rc != MPI_SUCCESS)
    {
        printf("Erro no start da aplicação.n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    //Determina o tamanho do grupo associado
    MPI_Comm_size(MPI_COMM_WORLD, &process_number);
    //Determina o rank do processo
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);

    if (rank_of_process == 0)
    {
        file = fopen(argv[1], "r");

        if (file == NULL)
        {
            printf("Erro na abertura do arquivo\n");
            exit(-1);
        }

        fscanf(file, "%d", &number_of_elements);
        printf("\nQuantidade de elementos no arquivo de entrada: %d\n", number_of_elements);

        // Dimensionamento do chunk
        chunk_size = (number_of_elements % process_number == 0)
                         ? (number_of_elements / process_number)
                         : (number_of_elements / process_number) + 1;

        data = (int *)malloc(process_number * chunk_size * sizeof(int));

        // Assign de cada elemento no arquivo para o vetor data
        for (int i = 0; i < number_of_elements; i++)
            fscanf(file, "%d", &data[i]);

        // Impressão de cada elemento do vetor
        printf("Elementos do vetor: \n");
        for (int i = 0; i < number_of_elements; i++)
            printf("%d  ", data[i]);
        printf("\n");

        fclose(file);
        file = NULL;
    }

    // Bloqueia todos os processos até atingirem esse ponto
    MPI_Barrier(MPI_COMM_WORLD);

    // O primeiro processo comunica a todos os outro o number_of_elements (tamanho do vetor)
    MPI_Bcast(&number_of_elements, 1, MPI_INT, 0,
              MPI_COMM_WORLD);

    // Dimensionamento do chunk
    chunk_size = (number_of_elements % process_number == 0)
                     ? (number_of_elements / process_number)
                     : (number_of_elements / process_number) + 1;

    // Alocação de espaço em memória para o vetor de chunk
    chunk = (int *)malloc(chunk_size * sizeof(int));

    // Envia partes diferentes do vetor data para todos os processos.
    // Essas partes são divididas de acordo com o chunk_size
    MPI_Scatter(data, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    free(data);
    data = NULL;

    // Dimensionamento do chunk de cada processo
    self_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 1))
                          ? chunk_size
                          : (number_of_elements - chunk_size * rank_of_process);

    q_sort(chunk, 0, self_chunk_size);

    if (rank_of_process != 0)
    {
        int destino = 0, tag = 0;
        MPI_Send(chunk, self_chunk_size, MPI_INT, destino, tag, MPI_COMM_WORLD);
    }
    else
    {
        for (int origin = 1; origin < process_number; origin++)
        {
            int *chunk_received;
            chunk_received = (int *)malloc(chunk_size * sizeof(int));
            MPI_Recv(chunk_received, chunk_size, MPI_INT, origin, 0, MPI_COMM_WORLD, &status);

            data = merge(chunk, self_chunk_size, chunk_received, chunk_size);

            free(chunk);
            free(chunk_received);
            chunk = data;
            self_chunk_size = self_chunk_size + chunk_size;
        }
    }

    // Abre o arquivo de saída (output.txt)
    if (rank_of_process == 0)
    {
        file = fopen(argv[2], "w");

        if (file == NULL)
        {
            printf("Erro na abertura do arquivo.\n");
            exit(-1);
        }

        // Imprime o numero de elementos passados
        fprintf(file, "Quantidade de elementos no vetor: %d\n", self_chunk_size);

        // Imprime cada elemento após o sort
        for (int i = 0; i < self_chunk_size; i++)
            fprintf(file, "%d  ", chunk[i]);

        fclose(file);

        printf("\n\nResultado salvo no arquivo output.txt.\n");

        // Impressão do resultado
        printf("Quantidade de elementos no arquivo de saída: %d\n", number_of_elements);
        printf("Elementos do vetor após o sort: \n");

        for (int i = 0; i < number_of_elements; i++)
            printf("%d  ", chunk[i]);

        printf("\n\nQuick sorted %d ints em %d processos.", number_of_elements, process_number);
        end = MPI_Wtime();
        printf("\nDuração: %f secs\n", end - start);
    }
    MPI_Finalize();
    return 0;
}