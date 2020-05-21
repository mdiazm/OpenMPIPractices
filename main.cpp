#include <iostream>
#include <mpi.h>

/*  HELLO WORLD IN OpenMPI  */

int main(int argc, char **argv){
  // MPI initialization
  MPI_Init(&argc, &argv);

  // Get cardinal of the set of processes
  int size = MPI::COMM_WORLD.Get_size();

  // Number of the active process
  int rank = MPI::COMM_WORLD.Get_rank();

  // Salutation from every process
  std::cout << "Hello world from the process: " << rank
  << " | Total of processes: " << size
  << std::endl;

  // MPI finish
  MPI_Finalize();

  return 0;
}
