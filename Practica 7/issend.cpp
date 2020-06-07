#include <iostream>
#include <mpi.h>

/*  Practice 7  */

#define SIZE 40
#define BOUNDARY 25

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // Data
    int matrix[SIZE][SIZE];

    // Initialize first element
    matrix[0][0] = 0;

    // Loop
    for(int i = 0; ; i++){
        MPI_Request request;
        MPI_Status status;

        // Process with rank 0 starts sending, left ones starts receiving
        MPI_Issend(matrix, SIZE * SIZE, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD, &request);


        printf("Proceso %d envía dato %d al proceso %d\n", rank, matrix[0][0], rank + 1);

        MPI_Irecv(matrix, SIZE * SIZE, MPI_INT, (rank - 1) % size, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        //MPI_Recv(matrix, SIZE * SIZE, MPI_INT, (rank - 1) % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proceso %d recibe dato %d del proceso %d\n", rank, matrix[0][0], size -1);

        matrix[0][0]++;

        // When boundary is surpassed, finalize execution
        if (matrix[0][0] >= BOUNDARY){
            MPI_Abort(MPI_COMM_WORLD, 0);
            MPI_Finalize();
            return 0;
        }
    }

    // MPI finish
    MPI_Finalize();

    return 0;
}
