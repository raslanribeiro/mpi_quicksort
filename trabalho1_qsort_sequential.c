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

void quicksort(int *arr, int start, int end)
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

    quicksort(arr, start, index - start);
    quicksort(arr, index + 1, start + end - index - 1);
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
        printf("Desired number of arguments are not their in argv....\n");
        printf("2 files required first one input and second one output....\n");
        exit(-1);
    }

    int number_of_process, rank_of_process;
    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
        printf("Error in creating MPI program Terminating......\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);

    file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Error in opening file\n");
        exit(-1);
    }

    printf("Reading number of Elements From file ....\n");
    fscanf(file, "%d", &number_of_elements);
    printf("Number of Elements in the file is %d \n", number_of_elements);

    data = (int *)malloc(number_of_elements * sizeof(int));

    // Reading the rest elements in which operation is being performed
    printf("Reading the array from the file.......\n");
    for (int i = 0; i < number_of_elements; i++)
        fscanf(file, "%d", &data[i]);

    // Printing the array read from file
    printf("Elements in the array is : \n");
    for (int i = 0; i < number_of_elements; i++)
        printf("%d  ", data[i]);
    printf("\n");

    fclose(file);
    file = NULL;

    // Starts Timer
    time_taken -= MPI_Wtime();

    quicksort(data, 0, number_of_elements);

    // Stop the timer
    time_taken += MPI_Wtime();

    // Opening the other file as taken form input and writing it to the file and giving it as the output
    file = fopen(argv[2], "w");

    if (file == NULL)
    {
        printf("Error in opening file... \n");
        exit(-1);
    }

    // Printing total number of elements in the file
    fprintf(file, "Total number of Elements in the array : %d\n", number_of_elements);

    // Printing the value of array in the file
    for (int i = 0; i < number_of_elements; i++)
        fprintf(file, "%d  ", data[i]);

    // Closing the file
    fclose(file);

    printf("\n\n\n\nResult printed in output.txt file "
           "and shown below: \n");

    // For Printing in the terminal
    printf("Total number of Elements given as input : %d\n", number_of_elements);
    printf("Sorted array is: \n");

    for (int i = 0; i < number_of_elements; i++)
        printf("%d  ", data[i]);

    printf("\n\nQuicksort %d ints: %f secs\n", number_of_elements, time_taken);
    free(data);

    MPI_Finalize();
    return 0;
}