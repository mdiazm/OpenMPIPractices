#include <iostream>
#include <mpi.h>
#include <cmath>

#define ROOT_PROC 0

// Matrix definition
//const int ROWS = 10, COLS = 10;
//int A[ROWS][COLS], B[ROWS][COLS], C[ROWS][COLS];

const int SIZE = 10;
int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE * SIZE];

/**
* @author Miguel Díaz Medina
* @brief initialize a matrix given rows and cols.
*/
void fillMatrix(int matrix[][SIZE], int rows, int cols, int fillValue = 1){
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            matrix[i][j] = fillValue; // TODO change this
        }
    }
}

void initializeResultMatrix(int *arr, int rows, int cols){
    for(int i = 0; i < rows*cols; i++)
        arr[i] = 0;
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
* @param pos cell position of C matrix that must be calculated using A and B matrix.
*/
void multiply(int pos, int rank = 0){
    
    int rowNum = pos / SIZE; // SIZE: number of columns
    int columnNum = pos % SIZE; // SIZE: number of columns

    for (int k = 0; k < SIZE; ++k){
        C[pos] += A[rowNum][k] * B[k][columnNum];
    }

    // C[pos] = rank;
}

void printArrayAsMatrix(int *arr, int rows, int cols){
    for(int i = 0; i < rows*cols; i++){
        printf("%d\t", arr[i]);

        if((i+1) % cols == 0){
            printf("\n");
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
    if(rank == ROOT_PROC){
        fillMatrix(A, SIZE, SIZE);
        fillMatrix(B, SIZE, SIZE);
        initializeResultMatrix(C, SIZE, SIZE);
    }

    // Matrix multiplication
    MPI_Bcast(A, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);
    MPI_Bcast(B, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);
    MPI_Bcast(C, SIZE * SIZE, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);

    int numCells = SIZE * SIZE;
    int cellsPerWorker = std::ceil(numCells / workers);

    if (numCells < workers){
        MPI_Finalize();
    }

    // Last worker will have to calculate a fewer number of cells.
    if (rank == workers -1) {
        // If last worker
        for(int i = rank * cellsPerWorker; i < numCells; i++){
            multiply(i, rank);
        }

    } else {
        // Calculate multiplication depending on cellsPerWorker
        for(int i = rank * cellsPerWorker; i < rank * cellsPerWorker + cellsPerWorker; i++){
            multiply(i, rank);
        }
    }

    // Gather computed results
    MPI_Gather(C + rank * cellsPerWorker, cellsPerWorker, MPI_INT, C + rank * cellsPerWorker, cellsPerWorker, MPI_INT, ROOT_PROC, MPI_COMM_WORLD);

    if (workers % 2 != 0){
        // Calculate last position in main process in number of workers is odd.
        multiply(numCells -1);
    }

    if (rank == ROOT_PROC)
        printArrayAsMatrix(C, SIZE, SIZE);

    // MPI finish
    MPI_Finalize();

    return 0;
}
