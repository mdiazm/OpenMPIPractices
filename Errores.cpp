#include <iostream>
#include <mpi.h>
#include <unistd.h>

/*  Errores.cpp  */
#define DIMS 2
#define ITERATIONS 5
#define NUMROWS 4
#define NUMCOLS 4

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();


    // Enable error handlers
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    int errorCode;

    // Create new cartesian topology from new communicator
    MPI_Comm cartComm;
    int dims[DIMS] = {NUMROWS/* rows */, NUMCOLS /* cols */};
    int nDims[DIMS] = {0, 0}; // Not circular in any dimension
    errorCode = MPI_Cart_create(MPI_COMM_WORLD, 2, dims, nDims, true, &cartComm);

    // Do check
    if (errorCode != MPI_SUCCESS){
        char error_string[2048];
        int lengthOfErrorString;

        MPI_Error_string(errorCode, error_string, &lengthOfErrorString);
        fprintf(stderr, "%3d: %s\n", rank, error_string);
        MPI_Abort(MPI_COMM_WORLD, errorCode);
    }

    // Get ranks of the preceeding and following processes
    int ranks[2]; // {source, target}
    MPI_Cart_shift(cartComm, 1 /* direction */, 1 /* offset: positive is forward */, &ranks[0], &ranks[1]);

    // Get row id of each process
    int coords[2];
    MPI_Cart_get(cartComm, DIMS, dims, nDims, coords);

    // Replacement of the actions in practice 6: unnecessary
        
    // MPI finish
    MPI_Finalize();

    return 0;
}