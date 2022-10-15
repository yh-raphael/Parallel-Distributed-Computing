#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum;

void* runner (void* param)
{
    int i, upper = atoi(param);     // -> stdlib.h
    sum = 0;

    for (i = 1; i <= upper; i++)
        sum += i;

    fprintf(stdout, "Worker thread is done! \n");

    /* Tell the master thread that I am done. */
    pthread_exit(0);
}

int main(int argc, char* argv[])
{
    pthread_t tid;
    pthread_attr_t attr;

    if (argc != 2) {
        fprintf(stderr, "usage: a.out <integer value> \n");
        return -1;
    }
    if (atoi(argv[1]) < 0) {
        fprintf(stderr, "%d must be >= 0 \n", atoi(argv[1]));
        return -1;
    }

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, argv[1]);
    pthread_join(tid, NULL);                        /* wait the worker thread to exit */

    printf("sum = %d \n", sum);

    return 0;
}