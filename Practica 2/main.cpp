#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <cmath>

#define TAM 8

/*  Practice 4  */

int main(int argc, char **argv){

    // Data
    int data[TAM];

    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // Define new type of data.
    int blockTam = std::ceil(static_cast<float>(TAM) / size);
    
    MPI_Datatype block;
    MPI_Type_contiguous(blockTam, MPI_INT, &block);
    MPI_Type_commit(&tipo);
    
    
    // MPI finish
    MPI_Finalize();

    return 0;
}
