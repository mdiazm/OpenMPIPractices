#include <iostream>
#include <mpi.h>

/*  Practice 7  */

#define SIZE 10
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

    // Only process with rank == 0 initializes the matrix
    if (rank == 0){
        // Initialize
        matrix[0][0] = 0;
    }

    // Loop
    for(int i = 0; ; i++){
        
        if (rank == 0){
            // Process with rank 0 starts sending, left ones starts receiving
            MPI_Isend(matrix, SIZE * SIZE, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            printf("Proceso %d envía dato %d al proceso %d\n", rank, matrix[0][0], rank + 1);

            MPI_Irecv(matrix, SIZE * SIZE, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Proceso %d recibe dato %d del proceso %d\n", rank, matrix[0][0], size -1);

            matrix[0][0]++;
        } else if (rank == (size - 1)){
            // Process with last rank sends to first process
            MPI_Irecv(matrix, SIZE * SIZE, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Proceso %d recibe dato %d del proceso %d\n", rank, matrix[0][0], rank - 1);

            matrix[0][0]++;

            MPI_Isend(matrix, SIZE * SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("Proceso %d envía dato %d al proceso %d\n", rank, matrix[0][0], 0);
        } else {
            // Rank belongs to [1, size-2]
            MPI_Irecv(matrix, SIZE * SIZE, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Proceso %d recibe dato %d del proceso %d\n", rank, matrix[0][0], rank - 1);

            matrix[0][0]++;

            MPI_Isend(matrix, SIZE * SIZE, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            printf("Proceso %d envía dato %d al proceso %d\n", rank, matrix[0][0], rank + 1); 
        }


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
