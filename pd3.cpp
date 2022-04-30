#include <mpi.h>

#include <iostream>
#define N 5
using namespace std;

void print(double array[], char id, double variable, char vid, int my_rank) {
    cout << "Modified: ----------------\n";
    cout << id << ": ";
    for (size_t i = 0; i < N; i++) {
        cout << array[i] << " ";
    }
    cout << "\n"
         << vid << ": " << variable << "\n";
    cout << "By P" << my_rank << "---------------------\n";
}

int main(int argc, char **argv) {
    double a[N], b[N], c[N], v = 0.0, w = 0.0;

    int my_rank;
    int size, i;
    //  MPI_Status stat;

    MPI_Init(&argc, &argv);                  /*inicia MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*RANK DEL PROCESO */
    MPI_Comm_size(MPI_COMM_WORLD, &size);    /* numero de procesos*/

    // if (size > 2) MPI_Abort(MPI_COMM_WORLD, -1);

    if (my_rank == 0) {  // P0
        // T1(a,v) -------------------
        for (size_t i = 0; i < N; i++)
            a[i] = 10;
        v = 100;
        print(a, 'a', v, 'v', my_rank);

        // T3(b,v) -------------------
        MPI_Recv(&b, N, MPI_DOUBLE, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "*Receiving in P" << my_rank << ". b: ";
        for (size_t i = 0; i < N; i++)
            cout << b[i] << " ";
        cout << "\n";

        for (size_t i = 0; i < N; i++)
            b[i] = 30;
        v = 300;
        print(b, 'b', v, 'v', my_rank);
        MPI_Send(&v, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        cout << "-Sending from P" << my_rank << ". v: " << v << "\n";

        // T6(a,w) -------------------
        MPI_Recv(&w, 1, MPI_DOUBLE, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "*Receiving in P" << my_rank << ". w: " << w << "\n";

        for (size_t i = 0; i < N; i++)
            a[i] = 60;
        w = 600;
        print(a, 'a', w, 'w', my_rank);

    } else if (my_rank == 1) {  // P1
        // T2(b,w) -------------------
        for (size_t i = 0; i < N; i++)
            b[i] = 20;
        w = 200;
        print(b, 'b', w, 'w', my_rank);
        MPI_Send(&b, N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        cout << "-Sending from P" << my_rank << ". b: ";
        for (size_t i = 0; i < N; i++)
            cout << b[i] << " ";
        cout << "\n";

        // T4(c,w) -------------------
        for (size_t i = 0; i < N; i++)
            c[i] = 40;
        w = 400;
        print(c, 'c', w, 'w', my_rank);
        MPI_Send(&w, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        cout << "-Sending from P" << my_rank << ". w: " << w << "\n";

        // T5(c,v) -------------------
        MPI_Recv(&v, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "*Receiving in P" << my_rank << ". v: " << v << "\n";

        for (size_t i = 0; i < N; i++)
            c[i] = 50;
        v = 500;
        print(c, 'c', v, 'v', my_rank);
    } else {  // other processes
        double a[N], b[N], c[N], v = 0.0, w = 0.0;
        cout << "Printing from P" << my_rank << ", which does not participate in the communication v = " << v << "w = " << w << endl;
    }
    MPI_Finalize(); /* salida MPI */
}
