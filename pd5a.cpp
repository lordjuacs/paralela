#include <limits.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctime>

/*
La probabilidad estadistica de que los N=5 amigos adivinen el mismo numero
random entre 1 al 100 intentandolo 1000 veces es 1000*(1/100)^5 = 1x10^-7 (1 entre 10 millones).
Al ejecutarlo con MPI se comprueba la probabilidad.

*/
int main(int argc, char *argv[]) {
    int N = 5;
    int rank, size, amigos[N], adivinado, i;
    int count = 0, check, times = 1000;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(time(NULL) + rank);

    for (int j = 0; j < times; j++) {
        if (rank == 0) {
            for (i = 0; i < size; i++)
                amigos[i] = 0;
        }
        MPI_Scatter(amigos, 1, MPI_INT, &adivinado, 1, MPI_INT, 0, MPI_COMM_WORLD);

        adivinado = rand() % 100 + 1;

        MPI_Gather(&adivinado, 1, MPI_INT, amigos, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            check = 0;
            for (i = 0; i < size; i++)
                if (amigos[i] == amigos[0])
                    check++;
            if (check == size)
                count++;
        }
    }
    if (rank == 0) {
        printf("Probability of same random numbers: %f\n", (double)count / times);
    }
    MPI_Finalize();

    return 0;
}