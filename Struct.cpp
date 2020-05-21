#include <iostream>
#include <mpi.h>

struct Particula {
    char c;
    double d[6];
    char b[7];
};

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    struct Particula enviado[10];
    struct Particula recibido[10];

    MPI_Datatype particulaTipo;
    MPI_Datatype tipos[3] = {MPI_CHAR, MPI_DOUBLE, MPI_CHAR};

    int longBloq[3] = {1, 6, 7};

    MPI_Aint desplaz[3];
    desplaz[0] = offsetof(Particula, c);
    desplaz[1] = offsetof(Particula, d);
    desplaz[2] = offsetof(Particula, b);

    MPI_Type_create_struct(3, longBloq, desplaz, tipos, &particulaTipo);
    MPI_Type_commit(&particulaTipo);

    if (rank == 0){
        enviado[0].c = '9'; // Ejemplo de dato enviado.
        MPI_Send(enviado, 10, particulaTipo, 1, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(recibido, 10, particulaTipo, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Ejemplo de dato recibido: %c\n", recibido[0].c);
    }

    // MPI finish
    MPI_Finalize();

    return 0;
}
