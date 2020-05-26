#include <iostream>
#include <mpi.h>


int main(int argc, char **argv){
  // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    MPI_Group orig_group, new_group;
    MPI_Comm new_comm;
    int *ranks = new int[size/2];

    // Select processes whose rank is even
    int cont = 0;
    for (int i = 0; i < size; i++) {
        if(i % 2 == 0){
            ranks[cont] = i;
            cont ++;
        }
    }

    MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
    MPI_Group_incl(orig_group, size/2, ranks, &new_group);
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);

    // MPI finish
    MPI_Finalize();

    delete [] ranks;

  return 0;
}