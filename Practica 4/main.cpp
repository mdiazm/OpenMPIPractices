#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <cmath>

#define TAM 10000
#define ROOT 0

void initialize(int * v){
    for(int i = 0; i < TAM; ++i){
        v[i] = i;
    }
}

int findMax(int *v, int num){
    int tmp = v[0];

    for(int i = 0; i < num; i++){
        if (v[i] > tmp)
            tmp = v[i];
    }

    return tmp;
}

/*  Practice 4  */

int main(int argc, char **argv){

    int *test;

    // Data
    int data[TAM];

    // MPI initialization
    MPI_Init(&argc, &argv);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // Initialize vector if it is necessary
    if(rank == ROOT)
        initialize(data);

    // Define new type of data.
    int blockTam = std::ceil(static_cast<float>(TAM) / size);
    int numInLast = TAM - blockTam * (size - 1);
    
    MPI_Datatype block;
    MPI_Type_contiguous(blockTam, MPI_INT, &block);
    MPI_Type_commit(&block);

    // Where to receive data
    int recv[TAM];

    int err = MPI_Alloc_mem(sizeof(int) * blockTam, MPI_INFO_NULL, &recv);

    // Distribute data in each process.    
    MPI_Scatter(&data, 1, block, &recv, blockTam, MPI_INT, ROOT, MPI_COMM_WORLD);

    int max = 0;
    // Print array for each process
    if (rank != size -1)
        max = findMax(recv, blockTam);
    else 
        max = findMax(recv, numInLast);

    int buffer;

    MPI_Reduce(&max, &buffer, 1, MPI_INT, MPI_MAX, ROOT, MPI_COMM_WORLD);

    if(rank == ROOT){
        printf("Maximum value of the set: %d\n", buffer);
    }

    // MPI finish
    MPI_Finalize();

    return EXIT_SUCCESS;
}
