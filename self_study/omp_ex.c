#include <stdio.h>
#include <omp.h>

void main()
{
    int c1 = 0; int c2 = 0;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp atomic
        c1++;
        printf("(a) %d\n", omp_get_thread_num());
        #pragma omp master
        {
            #pragma omp critical
            {
                c2++;
                printf("(b) %d\n", omp_get_thread_num());
            }
        }
        #pragma omp single
        {
            #pragma omp critical
            {
                printf("(c) %d\n", omp_get_thread_num());
                c2++;
            }
        }
    }
    printf("%d %d \n", c1, c2);
}