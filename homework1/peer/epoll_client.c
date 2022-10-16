#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>
#include <time.h>
#include "timer.h"

#define MAX_MSG_SIZE    1024


char server_ip[64];
int port_id;
int thread_count;
int access_count;
char list_file[64];

int file_count;
char* file_path[512];
long sum_received = 0;

double start, finish, elapsed;

int my_sock;
pthread_mutex_t lock;

void connect_socket();
void * do_request_routine (void * rank);


int main(int argc, char* argv[])
{
    /* Access .html file randomly */
    srand((unsigned int) time(NULL));

    /* Argument Vector Handling */
    if (argc != 6) {
        fprintf(stderr, "usage: a.out <server_ip> <server_port_id> <# of workers> <# of access> <list file path> \n");
        return -1;
    }
    if (atoi(argv[2]) < 0) {    // server_port_id
        fprintf(stderr, "%d must be >= 0 \n", atoi(argv[2]));
        return -1;
    }
    if (atoi(argv[3]) < 0) {    // # of workers
        fprintf(stderr, "%d must be >= 0 \n", atoi(argv[3]));
        return -1;
    }
    if (atoi(argv[4]) < 0) {    // # of access
        fprintf(stderr, "%d must be >= 0 \n", atoi(argv[4]));
        return -1;
    }

    strcpy(server_ip, argv[1]);
    port_id = atoi(argv[2]);
    thread_count = atoi(argv[3]);
    access_count = atoi(argv[4]);
    strcpy(list_file, argv[5]);


    /* Read a list file */
    FILE * fp = fopen(list_file, "rt");

    fscanf(fp, "%d\n", &file_count);
    if (file_count > 512) {
        fprintf(stderr, "Number of files in the list file cannot be greater than 512! \n");
        return -1;
    }
    for (int i = 0; i < file_count; i++)
    {
        char buffer[64];
        fscanf(fp, "%s\n", buffer);

        file_path[i] = (char*) malloc(strlen(buffer) + 1);
        strcpy(file_path[i], buffer);
    }
    fclose(fp);


    /* Try just one connect */
    GET_TIME(start);
    pthread_mutex_init(&lock, NULL);
	connect_socket();


    /* Generate a thread pool */
    long thread;
    pthread_t* thread_handles;
    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, do_request_routine, (void*) thread);
    }


    /* Wait for all worker threads */
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    close(my_sock);
    GET_TIME(finish);


    /* Print statistics */
    elapsed = finish - start;
    printf("Total received bytes: %ld \n", sum_received);
    printf("Client side elapsed time: %lf seconds \n", elapsed);

    
    /* Free Heap dynamic memory space */
    free(thread_handles);
    for (int i = 0; i < file_count; i++)
    {
        free(file_path[i]);
    }

    return 0;
}


/* Configure connection with the server */
void connect_socket()
{
	struct sockaddr_in serv_addr;

    my_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (my_sock == -1)
        printf("socket error \n");

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);
    serv_addr.sin_port = htons(port_id);

    if (connect(my_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
        printf("connect error \n");
}


void * do_request_routine (void * rank)
{
    char sent_message[MAX_MSG_SIZE];
    char recieved_message[MAX_MSG_SIZE];

    for (int i = 0; i < access_count; i++)
    {
        /* Make a HTTP request: [GET /tecfamoo/welcome.html HTTP/1.0] */
        int random = rand() % file_count;
        snprintf(sent_message, MAX_MSG_SIZE, "GET %s HTTP/1.0\r\n", file_path[random]);

        printf("Sent Message: %s \n", sent_message);

		pthread_mutex_lock(&lock);
        /* Send a HTTP request to the server */
        send(my_sock, sent_message, strlen(sent_message), 0);


        /* Receive a HTTP response from the server */
        if (recv(my_sock, recieved_message, MAX_MSG_SIZE, 0) == -1) {
            printf("receive error \n");
        }
        else {
            printf("Received Message: %s \n", recieved_message);
            sum_received += strlen(recieved_message);
        }
		pthread_mutex_unlock(&lock);

        // close(my_sock);
        sleep(2);
    }


    pthread_exit(0);
}