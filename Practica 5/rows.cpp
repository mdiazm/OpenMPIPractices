#include <iostream>
#include <mpi.h>
#include <cmath>

#define ROOT_PROC 0

// Treat matrix as one-dimensional arrays.
// Matrix definition: Square matrix
const int SIZE = 10;
int A[SIZE*SIZE], B[SIZE*SIZE], C[SIZE*SIZE];

void initializeMatrix(int * m, int tam, int fillValue = 0){
    for(int i = 0; i < tam; i++){
        if (fillValue != 0){
            m[i] = fillValue;
            continue;
        }
        
        int row = i/SIZE;
        int col = i % SIZE;

        if (row == col){
            m[i] = 1;
        } else {
            m[i] = 0;
        }
    }
}

void printArray(int *arr, int tam){
    for(int i = 0; i < tam; i++){
        std::cout << arr[i] << "\t";
    }

    std::cout << std::endl;
}

void printMatrix(int *matrix, int tam){
    for(int i = 0; i < tam; i++){
        std::cout << matrix[i] << "\t";
        if((i + 1) % SIZE == 0){
            std::cout << std::endl;
        }
    }
}

void multiply(int *A, int*B, int* C, int rowSize, int numCols){
    for (int j = 0; j < numCols; j++){
        C[j] = 0;
        for(int i = 0; i < rowSize; i++){
            int bPosition = rowSize * i + j;
            C[j] += A[i] * B[bPosition];
        }
    }
}

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int workers = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // Finalize if number of workers is not the same as number of rows
    int numberOfRows = SIZE;
    int numberOfCols = SIZE;
    if (numberOfRows != workers){
        MPI_Finalize();
        return 0;
    }

    // Initialize matrix in ROOT_PROCESS
    if(rank == ROOT_PROC){
        initializeMatrix(A, SIZE * SIZE, 1);
        initializeMatrix(B, SIZE * SIZE, 8);
    }

    // Send rows to each computing node
    MPI_Datatype row;
    MPI_Type_contiguous(numberOfCols, MPI_INT, &row);
    MPI_Type_commit(&row);

    // Row to receive
    int recv[numberOfCols];

    // Distribute A matrix
    MPI_Scatter(&A, 1, row, &A, SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);
    
    // Broadcast B matrix
    MPI_Bcast(B, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);

    // Perform multiplication
    multiply(A, B, C, numberOfCols, numberOfCols);



    // Catch results on ROOT_PROC
    MPI_Gather(&C, SIZE, MPI_INT, &C + rank * SIZE, SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);

    // Print result matrix on root_proc
    if (rank == ROOT_PROC){
        printMatrix(C, SIZE * SIZE);
    }

    // MPI finish
    MPI_Finalize();

    return 0;
}
