#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc,char *argv[])
{
	int rank, size, k=4, valor=0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank==2) valor=10;
//	if(rank==0)
	printf("Peekaboo! desde %d de %d con valor %d\n", rank, size,valor);

	MPI_Finalize();
}

