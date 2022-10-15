#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count;

void * Hello(void* rank);

int main(int argc, char* argv[])
{
    long thread;    printf("size of long: %d", sizeof(long));
    pthread_t* thread_handles;                                      // -> pthread.h

    /* 커맨드라인으로부터 스레드의 숫자를 얻는다. */
    thread_count = malloc(thread_count * sizeof(pthread_t));        // -> stdlib.h

    return 0;
}

void* Hello(void* rank)
{
    long my_rank = (long) rank;

    printf("Hello from thread %ld of %d\n", my_rank, thread_count);

    return NULL;
}