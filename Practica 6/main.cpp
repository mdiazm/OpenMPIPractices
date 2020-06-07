#include <iostream>
#include <mpi.h>
#include <unistd.h>

/*  Practice 6  */
#define DIMS 2
#define ITERATIONS 5

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // If size != 16 (4 * 4) processes, finalize
    if (size != (4 /*rows*/ * 4 /*cols*/)){
        MPI_Finalize();
    }

    // Create new cartesian topology from new communicator
    MPI_Comm cartComm;
    int dims[DIMS] = {4/* rows */, 4 /* cols */};
    int nDims[DIMS] = {0, 0}; // Not circular in any dimension
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, nDims, true, &cartComm);

    // Get ranks of the preceeding and following processes
    int ranks[2]; // {source, target}
    MPI_Cart_shift(cartComm, 1 /* direction */, 1 /* offset: positive is forward */, &ranks[0], &ranks[1]);

    // Get row id of each process
    int coords[2];
    MPI_Cart_get(cartComm, DIMS, dims, nDims, coords);

    // Different action depending on row
    int sendData; // buffers to send an receive data
    int recvData;

    for(int i = 0; i < ITERATIONS; i++){
        switch(coords[1]){
            case 0:
                // Generate random number
                srand(rank);
                sendData = rand()%100 +1; // Random number between 1 and 100
                MPI_Send(&sendData, 1, MPI_INT, ranks[1], 0, cartComm);

                printf("%d: Soy el proceso %d en fila %d y columna %d, y envío %d al proceso %d\n", i+1, rank, coords[1], coords[0], sendData, ranks[1]);
                break;
            case 1: 
                // Divive by 2
                MPI_Recv(&recvData, 1, MPI_INT, ranks[0], 0, cartComm, MPI_STATUS_IGNORE);
                sendData = recvData / 2;
                MPI_Send(&sendData, 1, MPI_INT, ranks[1], 0, cartComm);

                printf("%d: Soy el proceso %d en fila %d y columna %d. Recibo %d y envío %d al proceso %d\n", i+1, rank, coords[1], coords[0], recvData, sendData, ranks[1]);

                break;
            case 2:
                // Sum 100
                MPI_Recv(&recvData, 1, MPI_INT, ranks[0], 0, cartComm, MPI_STATUS_IGNORE);
                sendData = recvData + 100;
                MPI_Send(&sendData, 1, MPI_INT, ranks[1], 0, cartComm);

                printf("%d: Soy el proceso %d en fila %d y columna %d. Recibo %d y envío %d al proceso %d\n", i+1, rank, coords[1], coords[0], recvData, sendData, ranks[1]);

                break;
            case 3:
                // Multiply by 2
                MPI_Recv(&recvData, 1, MPI_INT, ranks[0], 0, cartComm, MPI_STATUS_IGNORE);
                sendData = recvData * 2;
                MPI_Send(&sendData, 1, MPI_INT, ranks[1], 0, cartComm);      

                printf("%d: Soy el proceso %d en fila %d y columna %d. Recibo %d y no envío nada\n", i+1, rank, coords[1], coords[0], recvData);    
                break;
        }

        MPI_Barrier(cartComm);
        sleep(2); // Delay
    }


        
    // MPI finish
    MPI_Finalize();

    return 0;
}