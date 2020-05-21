#include <iostream>
#include <mpi.h>

#define ROOT_PROC 0

// Matrix definition
//const int ROWS = 10, COLS = 10;
//int A[ROWS][COLS], B[ROWS][COLS], C[ROWS][COLS];

const int SIZE = 10;
int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

/**
* @author Miguel Díaz Medina
* @brief initialize a matrix given rows and cols.
*/
void fillMatrix(int matrix[][SIZE], int rows, int cols){
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            matrix[i][j] = 1; // TODO change this
        }
    }
}

/**
* @author Miguel Díaz Medina
* @brief print a matrix.
*/
void printMatrix(int matrix[][SIZE], int rows, int cols){
    for(int i = 0; i < rows; ++i){
        printf("(");
        for(int j = 0; j < cols; ++j){
            printf("%d\t", matrix[i][j]);
        }
        printf(")\n");
    }
}

/**
* @author Miguel Díaz Medina
* @brief Perform matrix multiplication.
*/
void multiply(int istart, int iend){
    for(int i = istart; i <= iend; ++i){
        for(int j = 0; j < SIZE; ++j){
            for(int k = 0; k < SIZE; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
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

    // Initialize matrix
    fillMatrix(A, SIZE, SIZE);
    fillMatrix(B, SIZE, SIZE);

    // Matrix multiplication
    MPI_Bcast(A, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);
    MPI_Bcast(B, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);
    MPI_Bcast(C, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);
    
    int istart, iend;

    istart = (SIZE / workers) * rank;
    iend = (SIZE / workers) * (rank + 1) -1;

    multiply(istart, iend);

    // Gather computed results
    MPI_Gather(C + (SIZE / workers * rank), SIZE*SIZE/workers, MPI_INT, C + (SIZE / workers * rank), SIZE*SIZE/workers, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);

    if(rank == 0){
        if(SIZE % workers > 0){
            multiply((SIZE/workers) * workers, SIZE - 1);
        }
        
        printMatrix(C, SIZE, SIZE);
    }



    // MPI finish
    MPI_Finalize();

    return 0;
}
