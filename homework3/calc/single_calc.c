#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 100

typedef struct _para
{
	double a;
	double b;
    char c;
} Para;

// long: 8byte, long long: 8byte
char convert_operator (char *);
double do_calculation (Para);
double calculate_expression (char *);

int main (int argc, char *argv[ ])
{
    // Get command-line arguments.
	if ( argc != 1 ) {
		fprintf(stderr, "Usage: ./calc_client \n");
		exit(1);
	}
    
    char arith[MAX_STR_LEN];
    scanf("%s", arith);
    //strcpy (arith, argv[1]);

    double result;
    result = calculate_expression (arith);  // Remote Procedure Call 1.
    printf("The answer is %d\n", (int) result);
    printf("The answer is %f in floating number\n", result);

    return 0;
}

char convert_operator (char * my_oper)
{
    if (!strcmp(my_oper, "+"))
        return '+';

    else if (!strcmp(my_oper, "-"))
        return '-';

    else if (!strcmp(my_oper, "*"))
        return '*';

    else if (!strcmp(my_oper, "/"))
        return '/';

    else if (!strcmp(my_oper, "**"))
        return '^';      // exponent -> 1byte char
}

double do_calculation (Para p)
{
    double answer = 1.0;
    switch (p.c)
    {
    case '+':
        return p.a + p.b;
    case '-':
        return p.a - p.b;
    case '*':
        return p.a * p.b;
    case '/':
        return p.a / p.b;
    case '^':
        for (int i = 0; i < (int) p.b; i++)     // due to math.h pow() binding problem.
            answer *= p.a;
        return answer;
    }
}

double calculate_expression (char * arith)
{
    char arith1[MAX_STR_LEN];
    char arith2[MAX_STR_LEN];
    strcpy(arith1, arith);
    strcpy(arith2, arith);

    int num_arr[MAX_STR_LEN];

    double stack_num[MAX_STR_LEN] = {0.0, };          // Final Result should be floating number.
    char stack_oper[MAX_STR_LEN] = {'\n', };
    int top_num = 0;
    int top_oper = 0;

    // [1] Separate numbers.
    int cnt = 0;
    char *my_num;
    my_num = strtok (arith1, "+-*/");
    while (my_num != NULL)
    {
        //printf("my_num: %s \n", my_num);
        num_arr[cnt++] = atoi (my_num);
        //num_arr[cnt++] = strtol (my_num, NULL, 10);
        my_num = strtok (NULL, "+-*/");
    }
    // for (int i = 0; i < cnt; i++)
    //     printf ("num_arr[%d] = %d \n", i, num_arr[i]);

    // [2] Separate operators - Read the entire string.
    int cnt2 = 0;
    char *my_oper;
    my_oper = strtok (arith2, "0123456789");
    stack_oper[top_oper ++] = convert_operator (my_oper); //printf("1) %c \n", stack_oper[top_oper - 1]);
    stack_num [top_num  ++] = (double) num_arr[cnt2 ++]; //printf("2) %f \n", stack_num[top_num - 1]);
    stack_num [top_num  ++] = (double) num_arr[cnt2 ++]; //printf("3) %f \n", stack_num[top_num - 1]);

    while (my_oper != NULL)
    {
        my_oper = strtok (NULL, "0123456789");
        if (my_oper == NULL) break;                 // The end of reading.
            
        char comp = convert_operator (my_oper);
        if (comp == '^')    
        {
            // Process right away!
            Para p; p.a = stack_num[top_num - 1]; p.b = (double) num_arr[cnt2 ++]; p.c = comp;
            stack_num[top_num - 1] = do_calculation (p);
            continue;
        }
        else if (comp == '+' || comp == '-')
        {
            // Do calculate all previous pairs!!
            while (1)
            {
                Para p; p.a = stack_num[top_num - 2]; p.b = stack_num [top_num - 1]; p.c = stack_oper[top_oper - 1];
                stack_num[top_num - 2] = do_calculation (p);
                top_num --;
                top_oper --;
                if (top_oper == 0) break;
            }

            // Store operator and number.
            stack_oper[top_oper ++] = comp;
            stack_num[top_num ++] = (double) num_arr[cnt2 ++];

            // printf("4) %f \n", stack_num[top_num - 1]);
            // printf("5) %c \n", stack_oper[top_oper - 1]);
            // printf("6) %f \n", stack_num[top_num - 1]);

        }
        else if (comp == '*' || comp == '/')
        {
            // Do calculate previous pair when the !
            if (stack_oper[top_oper - 1] == '*' || stack_oper[top_oper - 1] == '/')
            {
                Para p; p.a = stack_num[top_num - 2]; p.b = stack_num [top_num - 1]; p.c = stack_oper[top_oper - 1];
                stack_num[top_num - 2] = do_calculation (p);
                top_num --;
                top_oper --;
            }

            // Store operator and number.
            stack_oper[top_oper ++] = comp;
            stack_num[top_num ++] = (double) num_arr[cnt2 ++];

            // printf("4) %f \n", stack_num[top_num - 1]);
            // printf("5) %c \n", stack_oper[top_oper - 1]);
            // printf("6) %f \n", stack_num[top_num - 1]);

        }
    }
    /*
    printf("cnt2 = %d \n", cnt2);
    int num = top_num; int oper = top_oper;
    while (num) {
        num --;
        printf("stack_num[%d] = %f \n", num, stack_num[num]);
    }
    while (oper) {
        oper --;
        printf("stack_oper[%d] = %c \n", oper, stack_oper[oper]);
    }
    */

    // [3] Process the remains.
    // printf("%d, %d \n", top_num, top_oper);
    while (1)
    {
        Para p; p.a = stack_num[top_num - 2]; p.b = stack_num [top_num - 1]; p.c = stack_oper[top_oper - 1];
        stack_num[top_num - 2] = do_calculation (p);
        top_num --;
        top_oper --;
        if (top_oper == 0) break;
    }
    // printf("%d, %d \n", top_num, top_oper);
    // if (top_num == 1) printf("So far, so good! \n");

    return stack_num[top_num - 1];
}