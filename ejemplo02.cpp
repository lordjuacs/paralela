#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {

  int my_rank; 
  int size, i,ti,number;
//  MPI_Status stat;
  
  MPI_Init(&argc, &argv); /*inicia MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); /*RANK DEL PROCESO */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* numero de procesos*/

if(size>2) MPI_Abort(MPI_COMM_WORLD, -1);

  number=8;

  if (my_rank ==0) {
	number=-1;
	MPI_Send(&number,1,MPI_INT,1,0,MPI_COMM_WORLD); 
cout<<"Imprimiendo desde el rank "<<my_rank<<" que envia number= "<<number<<endl; 
}
  else if (my_rank==1){
	MPI_Recv(&number,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//	MPI_Recv(&number,1,MPI_INT,0,0,MPI_COMM_WORLD,&stat);
cout<<"Imprimiendo desde el rank "<<my_rank<<" que recibe number= "<<number<<endl; 
  }
else  
cout<<"Imprimiendo desde el rank "<<my_rank<<" que no participa en la comunicacion, y tiene a number= "<<number<<endl;
 
MPI_Finalize();  /* salida MPI */
  
}
