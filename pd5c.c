#include <omp.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#define N 1000

void leer(int A[N][N], int v[N]) {
    int i, j;
#pragma omp parallel for private(i, j)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = i + j;
        }
    }
#pragma omp parallel for private(i)
    for (i = 0; i < N; i++) {
        v[i] = i;
    }
}

void escribir(int x[N]) {
    for (int i = 0; i < N; i++) {
        std::cout << x[i] << " ";
    }
    std::cout << std::endl;
}

void execute() {
    int i, j;
    int A[N][N], v[N], x[N];
    leer(A, v);
#pragma omp parallel for private(i, j)
    for (i = 0; i < N; i++) {
        x[i] = 0;
        for (j = 0; j < N; j++) x[i] += A[i][j] * v[j];
    }
    // print A and v
    /*std::cout << "A:" << std::endl;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) std::cout << A[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << "v:" << std::endl;
    for (i = 0; i < N; i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl
              << "x:" << std::endl;
    */
    // escribir(x);
}
int main(int argc, char* argv[]) {
    int z = atoi(argv[1]);
    omp_set_num_threads(z);
    // get time before and after execution
    double start = omp_get_wtime();
    execute();
    double end = omp_get_wtime();
    std::cout << "Time(" << z << "): " << end - start << std::endl;

    return 0;
}