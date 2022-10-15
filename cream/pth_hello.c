#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count;

void * Hello(void* rank);

int main(int argc, char* argv[])
{
    long thread;                                                    // size of long: 8
    pthread_t* thread_handles;                                      // -> pthread.h

    /* 커맨드라인으로부터 스레드의 숫자를 얻는다. */
    thread_count = strtol(argv[1], NULL, 10);

    thread_handles = malloc(thread_count * sizeof(pthread_t));      // -> stdlib.h

    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, Hello, (void*)thread);

    printf("Hello from the main thread\n");

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    free(thread_handles);
    return 0;
}   /* main */

void* Hello(void* rank)
{
    long my_rank = (long) rank;

    printf("Hello from thread %ld of %d\n", my_rank, thread_count);

    return NULL;
}   /* Hello */