#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void Hello(void);   /* 스레드 함수 */

int main(int argc, char* argv[])
{
    /* 명령어 라인에서 몇 개의 스레드를 생성할지 입력한다. */
    int thread_count = strtol(argv[1], NULL, 10);

#   pragma omp parallel num_threads(thread_count)
    Hello();

    return 0;
} /* main */

void Hello(void)
{
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    printf("Hello from thread %d of %d \n", my_rank, thread_count);
} /* Hello */