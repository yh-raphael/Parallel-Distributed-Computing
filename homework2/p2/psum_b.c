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
int arr_tmp[PROBLEM_SIZE];

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

    /* START prefix sums */
    long sum = 0;
    /* master process */
    if (my_rank == 0) {
        int index, i;
        elements_per_process = PROBLEM_SIZE / num;
  
        // check if more than 1 processes are run
        if (num > 1) {
            // distributes the portion of array
            // to child processes to calculate
            // their partial sums
            for (i = 1; i < num - 1; i++)
            {
                index = i * elements_per_process;
  
                MPI_Send (&elements_per_process,
                          1, MPI_INT, i, 0,
                          MPI_COMM_WORLD);
                MPI_Send (&arr[index],
                          elements_per_process,
                          MPI_INT, i, 1,
                          MPI_COMM_WORLD);
            }
  
            // last process adds remaining elements
            index = i * elements_per_process;
            int elements_left = PROBLEM_SIZE - index;
  
            MPI_Send (&elements_left,
                      1, MPI_INT,
                      i, 0,
                      MPI_COMM_WORLD);
            MPI_Send (&arr[index],
                      elements_left,
                      MPI_INT, i, 1,
                      MPI_COMM_WORLD);
        }
  
        // master process add its own sub array
        for (i = 0; i < elements_per_process; i++)
            sum += arr[i];

        /* print the result */
        printf ("rank: %d\n", my_rank);
        printf ("%ld ", sum);
        printf ("\n");
  
        // collects partial sums from other processes
        int tmp;
        for (i = 1; i < num; i++) {
            MPI_Recv (&tmp, 1, MPI_INT,
                      MPI_ANY_SOURCE, 2,
                      MPI_COMM_WORLD,
                      &status);
            int sender = status.MPI_SOURCE;
  
            sum += tmp;
        }
    }
    /* Worker processes */
    else {
        MPI_Recv (&n_elements_recieved,
                  1, MPI_INT, 0, 0,
                  MPI_COMM_WORLD,
                  &status);
  
        // stores the received array segment
        // in local array arr_tmp
        MPI_Recv (&arr_tmp, n_elements_recieved,
                  MPI_INT, 0, 1,
                  MPI_COMM_WORLD,
                  &status);
  
        // calculates its partial sum
        long partial_sum = 0;
        for (int i = 0; i < n_elements_recieved; i++)
            partial_sum += arr_tmp[i];
  
        // sends the partial sum to the root process
        MPI_Send (&partial_sum, 1, MPI_INT,
                  0, 2, MPI_COMM_WORLD);

        /* print the result */
        printf ("rank: %d\n", my_rank);
        printf ("%ld ", partial_sum);
        printf ("\n");
    }

    /* MPI exit */
    MPI_Finalize ();

    /* evaluate the latency */
	if (my_rank == 0) {
        // prints the final sum of array - Worker
        printf("Sum of array is : %ld\n", sum);

		gettimeofday (&t, NULL);
		finish = t.tv_sec + t.tv_usec / 1000000.0;
        elapsed = finish - start;
		printf ("The code to be timed took %e seconds\n", elapsed);
	}

	return 0;
}