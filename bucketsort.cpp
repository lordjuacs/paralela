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

void parallelBucketSort(float* arr, int& n, int bucket_size) {
    // float* bucket = new float[bucket_size];
    std::vector<float> bucket;
    MPI_Scatter(arr, n / bucket_size, MPI_FLOAT, bucket, n / bucket_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    sort(bucket.begin(), bucket.end());
    MPI_Gather(bucket, n / bucket_size, MPI_FLOAT, arr, n / bucket_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
    int i, n = pow(2, 4);
    float* randArray;

    srand((int)time(0));

    randArray = new float[n];

    for (int i = 0; i < n; ++i)
        randArray[i] = (float)rand() / (float)(RAND_MAX / 999.0);

    for (i = 0; i < n; ++i)
        printf("%1.2f, ", randArray[i]);
    printf("\n");

    // ordenar array en buckets
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // bucketSort(randArray, n, 8);
    parallelBucketSort(randArray, n, 8);
    for (i = 0; i < n; ++i)
        printf("%1.2f, ", randArray[i]);

    delete[] randArray;
}