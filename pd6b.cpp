#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <vector>

void bucketSort(double* arr, int& n, int bucket_size) {
    // Crear buckets
    std::vector<double> bucket[bucket_size];
    int i, j, bucket_index, index = 0;

    // asignar elementos a los buckets
    for (i = 0; i < n; ++i) {
        bucket_index = bucket_size * arr[i] / 1000;
        bucket[bucket_index].push_back(arr[i]);
    }

    // ordenar buckets
    for (j = 0; j < bucket_size; ++j)
        std::sort(bucket[j].begin(), bucket[j].end());

    // Concatenar buckets en arr[]
    for (i = 0; i < bucket_size; i++) {
        for (j = 0; j < bucket[i].size(); j++)
            arr[index++] = bucket[i][j];
    }
}

void swap(double* a, double* b) {
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double* arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(double* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void parallelBucketSort(double* arr, int n, int bucket_size, int rank) {
    /*
    Se implementó una asignación de buckets en un arreglo auxiliar para de esta forma,
    poder dividirlo mediante scatter para poder realizar los ordenamientos de forma paralelo,
    para posteriormente juntar cada uno de estos segmentos con un gather.
    Estando cada elemento asignado en su bucket de forma correctamente y siendo cada uno de
    estos ordenados de forma paralela, bastaría con juntar cada uno de los segmentos para resolver el problema.
    */

    std::vector<double> new_arr[bucket_size];
    int bucket_index = 0;
    for (int i = 0; i < n; ++i) {
        bucket_index = bucket_size * arr[i] / 1000;
        new_arr[bucket_index].push_back(arr[i]);
    }

    int max_elements = 0;
    for (int i = 0; i < bucket_size; ++i) {
        if (new_arr[i].size() > max_elements)
            max_elements = new_arr[i].size();
    }

    int extra = 0;
    for (int i = 0; i < bucket_size; ++i) {
        while (new_arr[i].size() < max_elements) {
            new_arr[i].push_back(-1);
            extra++;
        }
    }

    double* final_arr = new double[n + extra];
    int index = 0;
    for (int i = 0; i < bucket_size; ++i) {
        for (int j = 0; j < new_arr[i].size(); ++j) {
            final_arr[index++] = new_arr[i][j];
        }
    }

    n += extra;
    double* bucket = new double[n / bucket_size];

    MPI_Scatter(final_arr, n / bucket_size, MPI_DOUBLE, bucket, n / bucket_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();
    quickSort(bucket, 0, (n / bucket_size) - 1);
    double end = MPI_Wtime();
    printf("Tiempo de ordenamiento: %f\t Rank: %d\n", end - start, rank);

    MPI_Gather(bucket, n / bucket_size, MPI_DOUBLE, final_arr, n / bucket_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    index = 0;
    for (int i = 0; i < n; ++i) {
        if (final_arr[i] != -1) {
            arr[index++] = final_arr[i];
        }
    }
}

int main(int argc, char* argv[]) {
    int i, n = pow(2,19);
    int rank, size;
    double* randArray;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand((int)time(0));

    randArray = new double[n];

    for (int i = 0; i < n; ++i)
        randArray[i] = (double)rand() / (double)(RAND_MAX / 999.0);

    // if (rank == 0) {
    //     printf("desordenado:\t");
    //     for (i = 0; i < n; ++i)
    //         printf("%1.2f, ", randArray[i]);
    //     printf("\n");
    // }

    double start = MPI_Wtime();
    parallelBucketSort(randArray, n, size, rank);
    double end = MPI_Wtime();
    double time = end - start;
    if (rank == 0) {
        // printf("ordenado:\t");
        // for (i = 0; i < n; ++i)
        //     printf("%1.2f, ", randArray[i]);
        // printf("\n");
        printf("Tiempo: %f\n", time);
    }
    delete[] randArray;
    MPI_Finalize();
}