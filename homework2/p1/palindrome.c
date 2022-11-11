/* Author: Raphael
   Creation Date: 2022-11-09 
   Topic: PPM image file in P6 format processing
   Go for it to the masterpiece! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>

#define MAX_LINE        30000
#define MAX_STR_LEN     64


int create_pool (FILE* ifp, char** word_pool);
void reverse_string (char* arr, char* rev);
int is_palindrome (char** word_pool, const int word_count, char* arr, char** result_table, int* result_count);


int main (int argc, char* argv[])
{
    FILE *ifp, *ofp;


    int word_count = 0;
    char** word_pool;
    /* allocate memory space for dictionary */
    word_pool = (char **) malloc (sizeof (char *) * MAX_LINE);
    for (int i = 0; i < MAX_LINE; i++)
        word_pool[i] = (char *) malloc (sizeof (char) * MAX_STR_LEN);

    char word[MAX_STR_LEN];


    int result_count = 0;
    char** result_table;    
    result_table = (char **) malloc (sizeof (char *) * MAX_LINE);
    for (int i = 0; i < MAX_LINE; i++)
        result_table[i] = (char *) malloc (sizeof (char) * MAX_STR_LEN);
    

    char input_file[MAX_STR_LEN];
    char output_file[MAX_STR_LEN];

    double start, finish, elapsed;
    int re;

    /* Parse the command-line arguments */
    if (argc != 4) {
        fprintf (stderr, "usage: a.out <# of threads> <input_file> <output_file>\n");
        return -1;
    }
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

    /* Read and construct a dictionary for the words */
    word_count = create_pool(ifp, word_pool);   // printf("word_count: %d \n", word_count);

    /* Start the time measurement */
    start = omp_get_wtime ();


    /* The for-loop parallelism */
//#pragma omp parallel for //private(word)
    for (int i = 0; i < word_count; i++)
    {
        //#pragma omp critical
        {
        if (!strcmp (word_pool[i], "\n")) continue;
        strcpy (word, word_pool[i]);
        }
        // printf("word: %s \n", word);
        re = is_palindrome(word_pool, word_count, word, result_table, &result_count);   // printf("re: %d \n", re);
    }
    printf("Total %d entries are determined as palindromes. \n", result_count);


    /* Check */
    #pragma omp parallel for
    for (int i = 0; i < result_count; i++)
        printf("%s \n", result_table[i]);
    
    #pragma omp parallel for
    /* Record the result to the "ouput.txt" */
    for (int i = 0; i < result_count; i++)
        fprintf(ofp, "%s \n", result_table[i]);


    /* Finish the time measurement */
    finish = omp_get_wtime ();
    elapsed = finish - start;
    printf("The code to be timed took %e seconds\n", elapsed);


//#   pragma omp parallel num_threads(thread_count)
    //Hello();

    for (int i = 0; i < MAX_LINE; i++)
        free (word_pool[i]);

    for (int i = 0; i < MAX_LINE; i++)
        free (result_table[i]);

    fclose (ifp);
    fclose (ofp);
    return 0;
} /* main */


/* Construct a dictionary for words */
int create_pool (FILE* ifp, char** word_pool)
{
    int i = 0;

    while (fscanf (ifp, "%s\n", word_pool[i++]) != EOF);
    // printf("last: %s\n", word_pool[i]);
    // printf("second last: %s\n", word_pool[i-1]);
    // printf("Third last: %s\n", word_pool[i-2]);
    return i;
}

/* Make the reverse */
void reverse_string (char* arr, char* rev)
{
    strcpy (rev, arr);
    for (int i = 0; i < strlen(rev) / 2; i++)
    {
        char temp = rev[i];
        rev[i] = rev[strlen(rev) - 1 - i];
        rev[strlen(rev) - 1 - i] = temp;
    }
}

/* Determine whether it is palindrome and Record it to the result_table */
int is_palindrome (char** word_pool, const int word_count, char* arr, char** result_table, int* result_count)
{
    char rev[MAX_STR_LEN];

    /* make the reverse */
    reverse_string (arr, rev);

    /* Determine and Record */
    if (!strcmp (arr, rev)) {                       // If the reverse is the same.
        strcpy(result_table[(*result_count)], arr);   // printf("(1) arr: %s\n", arr);
        (*result_count) ++;
        return 1;
    }
    else {
        for (int i = 0; i < word_count; i++)
        {
            if (!strcmp (rev, word_pool[i])) {      // If the reverse is already in the dictionary.
                strcpy(result_table[(*result_count)], arr);   // printf("(2) arr: %s\n", arr);
                (*result_count) ++;
                return 1;
            }
        }
    }

    /* Not a palindrome */
    return 0;
}