 #include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    int errs = 0, err;
    int j, count;
    char *ap;

    MPI_Init( &argc, &argv );
    MPI_Errhandler_set( MPI_COMM_WORLD, MPI_ERRORS_RETURN );
    for (count=1; count < 128000; count *= 2)
    {
        err = MPI_Alloc_mem( count, MPI_INFO_NULL, &ap );
        if (err) {
            int errclass;
            /* An error of MPI_ERR_NO_MEM is allowed */
            MPI_Error_class( err, &errclass );
            if (errclass != MPI_ERR_NO_MEM) {
                errs++;
                printf("MPI_Alloc_mem failed.\n");
            }
        }
        else {
            /* Access all of this memory */
            for (j=0; j<count; j++) {
                ap[j] = (char)(j & 0x7f);
            }
            MPI_Free_mem( ap );
        }
    }
    MPI_Finalize();
    return 0;
}