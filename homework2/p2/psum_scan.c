/* Author: Raphael
   Creation Date: 2022-11-10 
   Topic: Prefix Sums using MPI with blocking calls */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define PROBLEM_SIZE 104857600
// #define PROBLEM_SIZE 12

struct timeval t;
double start, finish, elapsed;

int my_rank, num;
int elements_per_process, n_elements_recieved;

// int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
int arr[PROBLEM_SIZE];
int arr_re[PROBLEM_SIZE];

int main (int argc, char *argv[])
{
    /* generating n random integers */
    const int n = PROBLEM_SIZE;
    srand (time (NULL));                  // If this statement commented, it changes into deterministic version.
    for (int i = 0; i < n; i++)
        arr[i] = rand () % 64;

    /* MPI init */
    MPI_Status status;
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &num);

	elements_per_process = PROBLEM_SIZE / num;          // elements_per_process: # of jobs per process.

    /* Start time measurement */
	if (my_rank == 0) {
		gettimeofday (&t, NULL);
		start = t.tv_sec + t.tv_usec / 1000000.0;
	}


    //MPI_Request req;
    MPI_Scan (arr, arr_re, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    /* MPI exit */
    MPI_Finalize ();

    /* evaluate the latency */
	if (my_rank == 0) {
        // prints the final sum of array - Worker
        //printf("Sum of array is : %ld\n", sum);

		gettimeofday (&t, NULL);
		finish = t.tv_sec + t.tv_usec / 1000000.0;
        elapsed = finish - start;
		printf ("The code to be timed took %e seconds\n", elapsed);
	}

	return 0;
}