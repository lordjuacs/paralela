#include <mpi.h>
#include <stdio.h>

#define N (1 << 6)
#define data_type double

int size, partition;
double com_time = 0, op_time = 0;

void multiplicar(data_type A[N][N], data_type v[N], data_type x[N]) {
    // multiplicación matriz vector solo de la parte particionada
    for (int i = 0; i < partition; i++) {
        for (int j = 0; j < N; j++) {
            x[i] += A[i][j] * v[j];
        }
    }
}

void do_partition(data_type A[N][N], data_type v[N], data_type x[N]) {
    data_type x_partition[N / size], A_partition[N][N];
    double time = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    // separa en N*partition
    MPI_Scatter(A, N * partition, MPI_DOUBLE, A_partition, N * partition, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(v, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // tiempo de barrera+scatter+broadcast
    com_time += (MPI_Wtime() - time);
    time = MPI_Wtime();
    multiplicar(A_partition, v, x_partition);
    // tiempo de FLOPS
    op_time += (MPI_Wtime() - time);
    time = MPI_Wtime();
    // vuelve a juntar
    MPI_Gather(x_partition, partition, MPI_DOUBLE, x, partition, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // tiempo de gather
    com_time += (MPI_Wtime() - time);
}

void leer(data_type A[N][N], data_type v[N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i * j + 1;
        }
    }

    for (int i = 0; i < N; i++) {
        v[i] = i + 1;
    }
}

int main(int argc, char *argv[]) {
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    partition = N / size;

    data_type A[N][N], v[N], x[N];

    // proceso A lee
    if (rank == 0) leer(A, v);
    // realiza la partición
    do_partition(A, v, x);

    // proceso A tiene la rpta e imprime los datos
    if (rank == 0) {
        printf("Communication time : %f \n", com_time);
        printf("Operation time : %f \n", op_time);
        printf("Total time : %f \n", (op_time + com_time));
        // for (int i = 0; i < N; i++) printf("%f ", x[i]);
        // printf("\n");
    }

    MPI_Finalize();
}