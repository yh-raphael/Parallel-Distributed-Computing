#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// #include <netinet/tcp.h> 

#include "my_micro_httpd.h"
#include "queue.h"

/* Shared Resources */
Queue RQ;
Queue* request_queue;
int thread_count;

pthread_mutex_t mutex;
pthread_cond_t not_empty;

void * service_dispatch(void * rank);
int settcpnodelay(int sock);

/* Master Thread */
int main(int argc, char* argv[])
{
    int server_socket, client_socket, clilen;       // server_socket: listenfd, client_socket: connfd
    struct sockaddr_in serv_addr, cli_addr;         // -> netdb.h
    int one, port_id;
    // pthread_t service_thr;

    long thread;
    pthread_t* thread_handles;
    Request input_request;

    if (argc != 3) {
        fprintf(stderr, "usage: a.out <port_id> <# of workers> \n");
        return -1;
    }
    if (atoi(argv[1]) < 0) {
        fprintf(stderr, "%d must be >= 0 \n", atoi(argv[1]));
        return -1;
    }
    if (atoi(argv[2]) < 0) {
        fprintf(stderr, "%d must be >= 0 \n", atoi(argv[2]));
        return -1;
    }

    port_id = atoi(argv[1]);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)      // -> sys/types.h, sys/socket.h
    {            // 시스템이 listenfd를 반환해줌.
        printf("Error: Unable to open socket in parmon server. \n");
        exit(1);
    }

    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port_id);

    // setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one));     // ??? Why necessary? (from lecture notes)
    // settcpnodelay(server_socket);

    if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        // printf("Error: Unable to bind socket in parmon server -> %d \n", errno);
        printf("Error: Unable to bind socket in parmon server \n");
        exit(1);
    }

    listen(server_socket, 5);                                                               // ??? Parameter: "backlog" meaning?

    printf("Listening now . . . \n");

    /* Initialize a single request queue */
    request_queue = &RQ;
    InitQueue(request_queue);

    thread_count = atoi(argv[2]);
    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));      // -> stdlib.h

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_empty, NULL);

    /* Generate a thread pool */
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, service_dispatch, (void*) thread);
    }

    /* Receive all network I/O and deliver the requests to the workers */
    while (1)
    {

        clilen = sizeof(cli_addr);
        client_socket = accept(server_socket, (struct socketaddr *)&serv_addr, &clilen);

        if (client_socket < 0)
        {
            printf("connection to client failed in server. \n");
            continue;
        }

        /* Get user request */
        if (read(client_socket, &input_request, sizeof(Request)) > 0)                       // !!!!!!!!!!! msg 상세화?!
        {
            printf("Client Socket: %d -> Received Request: %s \n", client_socket, input_request.msg);

            /* Identify user request */
            input_request.fd = client_socket;
            Enqueue(request_queue, input_request);
        }

        /* Thread per request Method */
        // pthread_create(&service_thr, NULL, service_dispatch, client_socket);
        // pthread_detach(service_thr);    /* The detached thread can act as daemon thread. */
        //                                 /* The parent thread doesn't need to wait. */
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);

    free(thread_handles);
    close(server_socket);

    return 0;
}

/* Service(Worker) Thread */
void * service_dispatch(void * rank)
{
    Request popped;
    int rv;

    // /* Get user request */
    // if (read(client_socket, &input_request, sizeof(Request)) > 0)
    // {
    //     /* Identify user request */
    //     input_request.fd = client_socket;
    //     /* Do necessary processing, synchronize if necessary */
    //     /* Send results back to the server */
    // }

    /* Monitor the queue to process each request */
    while (1)
    {
        popped = Dequeue(request_queue);
        if (do_worker_routine(popped.msg, popped.fd) < 0)                   // refer to micro_httpd.h
        {
            fprintf(stderr, "Error: Unable to process HTTP request \n");
        }

        /* Close connect */
        close(popped.fd);
    }

    /* Close connect and terminate thread */
    // close(popped.fd);
    // pthread_exit((void *) 0);
}


// int settcpnodelay(int sock)
// {
// 	int flag = 1;
// 	int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
// 	return result;
// }