#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank, num, i;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num);
    printf("Hello from process %i of %i \n", rank, num);
    MPI_Finalize ();
    return 0;
}