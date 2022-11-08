#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAX_NAME_LEN    32

void Hello (void);   /* 스레드 함수 */

int main (int argc, char* argv[])
{
    FILE *ifp, *ofp;
    char input_file[MAX_NAME_LEN];
    char output_file[MAX_NAME_LEN];

    if (argc != 4) {
        fprintf (stderr, "usage: a.out <# of threads> <input_file> <output_file>\n");
        return -1;
    }
    /* 명령어 라인에서 몇 개의 스레드를 생성할지 입력한다. */
    int thread_count = strtol (argv[1], NULL, 10);
    if (thread_count <= 0) {
        fprintf(stderr, "%d must be > 0 \n", atoi(argv[1]));
        return -1;
    }
    strcpy(input_file, argv[2]);
    if ((ifp = fopen (input_file, "rt")) == NULL) {
        fprintf (stderr, "input file open error\n");
        return -1;
    }
    strcpy(output_file, argv[3]);
    if ((ofp = fopen (output_file, "wt")) == NULL) {
        fprintf (stderr, "output file open error\n");
        return -1;
    }

    /* 시작 */
    fprintf(ofp, "hello\n");



#   pragma omp parallel num_threads(thread_count)
    Hello();


    fclose (ifp);
    fclose (ofp);
    return 0;
} /* main */

void Hello (void)
{
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    printf("Hello from thread %d of %d \n", my_rank, thread_count);
} /* Hello */