#include <iostream>
#include <mpi.h>


int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    int blocklen[3] = {2, 3, 1};
    int displacement[3] = {1, 6, 3};
    int buffer[27];

    MPI_Datatype type;
    MPI_Datatype type2;

    MPI_Type_contiguous(3, MPI_INT, &type2);
    MPI_Type_commit(&type2);
    MPI_Type_indexed(3, blocklen, displacement, type2, &type);
    MPI_Type_commit(&type);

    for (int i = 0; i < 27; i++){
        buffer[i] = i;
    }

    if (rank == 0)
        MPI_Send(&buffer, 1, type, 1, 0, MPI_COMM_WORLD);
    else{
        MPI_Recv(&buffer, 18, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < 18; i++){
            std::cout << buffer[i] << " ";
        }
        std::cout << std::endl;
    }

    // MPI finish
    MPI_Finalize();

    return 0;
}
