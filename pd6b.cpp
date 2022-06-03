#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <vector>

void bucketSort(float* arr, int& n, int bucket_size) {
    // Crear buckets
    std::vector<float> bucket[bucket_size];
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

void swap(float* a, float* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(float* arr, int low, int high) {
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

void quickSort(float* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void parallelBucketSort(float* arr, int& n, int bucket_size, int rank) {
    /*
    Se implementó una asignación de buckets en un arreglo auxiliar para de esta forma,
    poder dividirlo mediante scatter para poder realizar los ordenamientos de forma paralelo,
    para posteriormente juntar cada uno de estos segmentos con un gather.
    Estando cada elemento asignado en su bucket de forma correctamente y siendo cada uno de
    estos ordenados de forma paralela, bastaría con juntar cada uno de los segmentos para resolver el problema.
    */

    std::vector<float> new_arr[bucket_size];
    int bucket_index = 0;
    for (int i = 0; i < n; ++i) {
        bucket_index = bucket_size * arr[i] / 1000;
        new_arr[bucket_index].push_back(arr[i]);
    }
    /*for (int i = 0; i < bucket_size; ++i) {
        for (int j = 0; j < new_arr[i].size(); j++) {
            printf("%1.2f, ", new_arr[i][j]);
        }
        printf("|");
    }
    printf("\n");*/
    if (rank == 0) {
        int cont = 0;
        for (auto& vec : new_arr) {
            for (int i = 0; i < vec.size(); i++) {
                arr[cont++] = vec[i];
            }
        }
    }
    float* bucket = new float[n / bucket_size];

    MPI_Scatter(arr, n / bucket_size, MPI_FLOAT, bucket, n / bucket_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    quickSort(bucket, 0, (n / bucket_size) - 1);
    /*for (int j = 0; j < n / bucket_size; j++) {
        printf("%1.2f, ", bucket[j]);
    }
    printf("\n");*/
    MPI_Gather(bucket, n / bucket_size, MPI_FLOAT, arr, n / bucket_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
    int i, n = pow(2, 4);  // para probarlo con 2^19 es mucho print en consola, se hara en la entrega final
    int rank, size;
    float* randArray;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand((int)time(0));

    randArray = new float[n];

    for (int i = 0; i < n; ++i)
        randArray[i] = (float)rand() / (float)(RAND_MAX / 999.0);

    if (rank == 0) {
        printf("desordenado:\t");
        for (i = 0; i < n; ++i)
            printf("%1.2f, ", randArray[i]);
        printf("\n");
    }

    // bucketSort(randArray, n, 8);
    if (rank == 0) {
        parallelBucketSort(randArray, n, size, rank);

        printf("ordenado:\t");
        for (i = 0; i < n; ++i)
            printf("%1.2f, ", randArray[i]);

        delete[] randArray;
    }
    MPI_Finalize();
}