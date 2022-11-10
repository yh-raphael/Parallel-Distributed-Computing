#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>

#define MAX_LINE        30000
#define MAX_NAME_LEN    64
#define MAX_STR_LEN     64

FILE *ifp, *ofp;
char word_pool[MAX_LINE][MAX_STR_LEN];
int count;

char rev[MAX_STR_LEN];
char arr[MAX_STR_LEN];

void Hello (void);   /* 스레드 함수 */

/* 인풋 파일 기반 word pool 생성 함수 */
void create_pool (void)
{
    int i = 0;

    while (fscanf (ifp, "%s\n", word_pool[i++]) != EOF);

    count = i;
}

void reverse_string (void)
{
    strcpy(rev, arr);    // arr 배열을 rev 배열에 복사
    for (int i = 0; i < strlen(rev) / 2; i++) {        // rev 배열의 반만 돌아도 된다
        // swap
        char temp = rev[i];
        rev[i] = rev[strlen(rev) - 1 - i];    // 배열이 0부터 시작하므로 -1을 해 줌
        rev[strlen(rev) - 1 - i] = temp;
    }
}

int is_palindrome (void)
{
    reverse_string();
    if (!strcmp (arr, rev)) {                       // reverse랑 같으면 palindrome.
        fprintf (ofp, "%s\n", arr);
        return 1;
    }
    else {
        for (int i = 0; i < count; i++) {
            if (!strcmp (rev, word_pool[i])) {      // reverse가 pool에 있으면 palindrome.
                fprintf (ofp, "%s\n", arr);
                return 1;
            }
        }
    }
    return 0;
}

int main (int argc, char* argv[])
{
    char input_file[MAX_NAME_LEN];
    char output_file[MAX_NAME_LEN];
    //struct timeval t;
    double start, finish, elapsed;
    //int re;

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
    create_pool();

    /* 병렬화 부분 */
    // gettimeofday(&t, NULL);
    // start = t.tv_sec + t.tv_usec / 1000000.0;
    start = omp_get_wtime ();

#   pragma omp parallel for
    for (int i = 0; i < count; i++) {
        strcpy (arr, word_pool[i]);
        //is_palindrome();
        //printf("re: %d \n", re);
        #pragma omp critical
        reverse_string();

        if (!strcmp (arr, rev)) {                       // reverse랑 같으면 palindrome.
            #pragma omp critical
            fprintf (ofp, "%s\n", arr);
            //return 1;
        }
        else {
            for (int i = 0; i < count; i++) {
                if (!strcmp (rev, word_pool[i])) {      // reverse가 pool에 있으면 palindrome.
                    #pragma omp critical
                    fprintf (ofp, "%s\n", arr);
                    //return 1;
                }
            }
        }
        //return 0;
    }

    // gettimeofday(&t, NULL);
    // finish = t.tv_sec + t.tv_usec / 1000000.0;
    finish = omp_get_wtime ();

    elapsed = finish - start;
    printf("The code to be timed took %e seconds\n", elapsed);



//#   pragma omp parallel num_threads(thread_count)
    //Hello();


    fclose (ifp);
    fclose (ofp);
    return 0;
} /* main */

// void Hello (void)
// {
//     int my_rank = omp_get_thread_num();
//     int thread_count = omp_get_num_threads();

//     printf("Hello from thread %d of %d \n", my_rank, thread_count);
// } /* Hello */

