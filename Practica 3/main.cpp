#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <cmath>

#define ROOT 0
#define RADIUS 0.5

/*  Practice 3. Calculate Pi number  */

int main(int argc, char **argv){

    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int numPoints = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int pid = MPI::COMM_WORLD.Get_rank();

    // Initialize seed for each process depending of time and PID
    srand(time(NULL) + pid);

    // Generate a random point: x [-RADIUS, RADIUS], y [-RADIUS, RADIUS]
    float x = -RADIUS + static_cast<float> (rand()) / (static_cast<float>(RAND_MAX/(2*RADIUS)));
    float y = -RADIUS + static_cast<float> (rand()) / (static_cast<float>(RAND_MAX/(2*RADIUS)));

    // Only choose those points that are inside the circle. x² + y² <= RADIUS²
    float distance = sqrt(pow(x, 2) + pow(y, 2));

    int contributes = 0;

    if(distance <= RADIUS) {
        contributes = 1;
    }

    int buffer = 0;

    MPI_Reduce(&contributes, &buffer, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

    if(pid == ROOT){
        float pi = 4 * static_cast<float>(buffer) / numPoints;
        printf("Buffer: %d\n", buffer);
        printf("PI: %lf\n", pi);
    }

    // MPI finish
    MPI_Finalize();

    return 0;
}
