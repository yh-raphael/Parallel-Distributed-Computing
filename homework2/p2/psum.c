#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// #include <time.h>

// #include <unistd.h>
// #include <math.h>
#define N 10000000

int main(int argc, char **argv){
	int array[N];
	int partial_result=0;
	int rank, num, i, recv;
	int size, quo, rem, base;
	int count, total_result;
	struct timeval nowtime, nowtime2;
	int s, s2;

	MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num);
	if(rank == 0){
		gettimeofday(&nowtime, NULL);
		s = nowtime.tv_sec;
	}

	//srand(time(NULL));
	//array[i] = rand()%10;
	for(i=0;i<N;i++){
		array[i]=i;
	}

	quo=N/num;
	rem=N%num;

	size = quo;
	//size is the number of parts of the array which each process have to deal with
	if(rank<rem) 
		size++; 

	for(i=0;i<size;i++){
		partial_result += array[num*i+ rank];
	}

	if(rank == 0){//master
		for(i=1;i<num;i++){
			MPI_Recv(&recv, 1, MPI_INT, i, i+24, MPI_COMM_WORLD, &status);
			partial_result += recv;
		}

		printf("collapse of a set of integers : %d\n",partial_result);//totals the partial sums

		//
		total_result=partial_result;
		while(total_result >= 10){
			count=1;
			total_result=0;

			while(partial_result >= 10){
				total_result += (partial_result%10);
				partial_result /= 10;
				count++;
			}
			total_result += (partial_result%10);

			//printf("count:%d\n",count);
			//printf("total_result:%d\n",total_result);
			partial_result = total_result;
		}
		
		printf("ultimate collapse of a set of integers : %d\n",total_result);
	}
	else //slaves pass its local sum to the master
		MPI_Send(&partial_result, 1, MPI_INT, 0, rank+24, MPI_COMM_WORLD);
	if(rank == 0){
		gettimeofday(&nowtime2, NULL);
		s2 = nowtime2.tv_sec;
		printf("ellapsed time is %d\n",s2-s);
	}

	MPI_Finalize();
	return 0;
}
