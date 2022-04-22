#include <mpi.h>  
#include <iostream>
using namespace std;

int main(int argc, char **argv) {

  int my_rank; 
  int size, i,ti,number,buf;
  MPI_Status stat;
  
  MPI_Init(&argc, &argv); /*inicio MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*determina el rank del proceso*/
  MPI_Comm_size(MPI_COMM_WORLD, &size); /*determina el numero de procesos*/

  if (my_rank ==0) {
	buf=1;
	for(i=1;i<size;i++)
		MPI_Send(&buf,1,MPI_INT,i,0,MPI_COMM_WORLD);
cout<<"Imprimiendo desde el rank "<<my_rank<<" que envia buf= "<<buf<<endl; 
}
  else {
buf=100;
	MPI_Recv(&buf,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	cout<<"Imprimiendo desde el rank "<<my_rank<<" que recibe buf= "<<buf<<endl; 
  }
MPI_Finalize();  /* salida MPI */
  
}
