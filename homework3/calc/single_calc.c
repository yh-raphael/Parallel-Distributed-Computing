#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 100

// long: 8byte, long long: 8byte
int convert_expression (char *);
long calculate_expression (char *);

int main (int argc, char *argv[ ])
{
    // Get command-line arguments.
	if ( argc != 2 ) {
		fprintf(stderr, "Usage: ./calc_client arithmetic_expression \n");
		exit(1);
	}
    
    char arith[MAX_STR_LEN];
    strcpy (arith, argv[1]);

    convert_expression (arith);             // Remote Procedure Call 1.

    int result;
    result = calculate_expression (arith);  // Remote Procedure Call 2.
    printf("%d\n", result);

    return 0;
}

int convert_expression (char * arith)
{
    //printf("%s %ld \n", arith, strlen(arith));
    char post[MAX_STR_LEN];

}

long calculate_expression (char * arith)
{
    int result = 0;
    return result;
}