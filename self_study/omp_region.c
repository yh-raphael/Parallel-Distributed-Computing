#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void test(int id, double* A);

int main(void)
{
    double A[1000];
    omp_set_num_threads(3);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        test(id, A);
    }
    printf("all done \n");

    return 0;
}

void test(int id, double* alloca)
{
    printf("I am %d ! \n", id);
}