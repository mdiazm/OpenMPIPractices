#include <iostream>
#include <mpi.h>
#include <unistd.h>

#define NUM_BOINGS 4

/*  Practice 2  */

int main(int argc, char **argv){

    int boings = NUM_BOINGS;

    // Configure number of boings using parameters
    if(argc == 2){
        boings = atoi(argv[1]);
    }

    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // If number of processes is not even, execution must finish.
    if(size % 2 != 0){
        MPI_Finalize();
        return 0;
    }

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // Init buffer
    int buffer = 1;

    // Identify the processes: even numbers start sending to odd ones
    if (rank % 2 == 0){
        for(int i = 0; i < boings; i++){
            // Send with blocking to peer process
            printf("Process %d sends %d\n", rank, buffer);
            MPI_Ssend(&buffer, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            // Wait until received
            
            // Receive data
            MPI_Recv(&buffer, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d receives %d\n", rank, buffer);
            // Add 1 to buffer and start Send again
            buffer++;
        }

    } else {
        for(int i = 0; i < boings; i++){
            // Receive data
            MPI_Recv(&buffer, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d receives %d\n", rank, buffer);
            sleep(1);
            // Add 1 to buffer
            buffer++;

            // Send data again
            printf("Process %d sends %d\n", rank, buffer);
            MPI_Ssend(&buffer, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
            // Wait until data is received by the peer process
        }
    }

    // MPI finish
    MPI_Finalize();

    return 0;
}
