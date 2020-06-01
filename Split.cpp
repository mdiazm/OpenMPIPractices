#include <iostream>
#include <mpi.h>

/*  Split.cpp */

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    MPI_Comm newGroup, comm;
    MPI_Comm_split(MPI_COMM_WORLD, rank % 2, rank, &newGroup);
    MPI_Intercomm_create(newGroup, 0, MPI_COMM_WORLD, 1 - rank%2, 0, &comm);

    int newRank = 0;
    MPI_Comm_rank(comm, &newRank);

    int msg[2] = {rank, newRank};
    printf("Proceso con rank global %d y rank local %d envía al proceso homónimo en el otro comunicador usando el intercomunicador los datos %d %d\n", rank, newRank, msg[0], msg[1]);
    MPI_Send(msg, 2, MPI_INT, newRank, 0, comm);

    int recv[2];

    MPI_Recv(recv, 2, MPI_INT, newRank, 0, comm, MPI_STATUS_IGNORE);
    printf("Proceso con rank global %d y rank local %d recibe del proceso homónimo en el otro comunicador usando el intercomunicador los datos %d %d\n", rank, newRank, recv[0], recv[1]);

    // MPI finish
    MPI_Finalize();

    return 0;
}
