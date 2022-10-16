#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <netinet/tcp.h> 
#include <fcntl.h> 

#include <sys/epoll.h>
#include <sys/ioctl.h> 

#include "my_micro_httpd.h"

#define EPOLL_SIZE 20

// #define __DEBUG__


/* Shared Resources */
// Queue RQ;
// Queue* request_queue;
int thread_count;

// pthread_mutex_t mutex;
// pthread_cond_t not_empty;


int server_socket, client_socket, clilen;       // server_socket: listenfd, client_socket: connfd

int efd;
int first_flag = 0;
pthread_cond_t first;
pthread_mutex_t epoll_mutex;
pthread_mutex_t rw_mutex;

void * service_dispatch(void * rank);
int nonblock(int sock);
int settcpnodelay(int sock);


/* Main Thread */
int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, cli_addr;         // -> netdb.h
    int one, port_id;
    // pthread_t service_thr;

    long thread;
    pthread_t* thread_handles;
    // Request input_request;

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
    thread_count = atoi(argv[2]);
    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));      // -> stdlib.h



	pthread_mutex_init(&epoll_mutex, NULL);
	pthread_mutex_init(&rw_mutex, NULL);
	pthread_cond_init(&first, NULL);

    int n;
    struct epoll_event *events;
    struct epoll_event event;
	// 이벤트 풀의 크기만큼 events구조체를 생성j.
	events = (struct epoll_event *) malloc(sizeof(struct epoll_event) * EPOLL_SIZE); 
	// epoll_create를 이용해 epoll descriptor를 생성.
	if ((efd = epoll_create(100)) < 0) { 
		fprintf(stderr, "epoll_create error");
		return -1;
	}



    /* Generate a thread pool */
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, service_dispatch, (void*) thread);
        pthread_detach(thread_handles[thread]);
    }


    /* Open the socket */
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

    if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        // printf("Error: Unable to bind socket in parmon server -> %d \n", errno);
        printf("Error: Unable to bind socket in parmon server \n");
        exit(1);
    }

    listen(server_socket, 5);                                                               // ??? Parameter: "backlog" meaning?

    printf("Listening now . . . \n");

    // /* Initialize a single request queue */
    // request_queue = &RQ;
    // InitQueue(request_queue);

    // pthread_mutex_init(&mutex, NULL);
    // pthread_cond_init(&not_empty, NULL);



    // 만들어진 듣기 소켓을 epoll이벤트 풀에 등록 
	// EPOLLIN (read) 이벤트의 발생을 탐지. 
	event.events = EPOLLIN; 
	event.data.fd = server_socket; 
	epoll_ctl(efd, EPOLL_CTL_ADD, server_socket, &event); 


    clilen = sizeof(cli_addr);
    client_socket = accept(server_socket, (struct socketaddr *)&serv_addr, &clilen);
    if (client_socket < 0) {
        printf("connection to client failed in server. \n");
        return -1;
    }

#ifdef __DEBUG__
    printf("new client accepted\n");
#endif

	nonblock(client_socket);
	settcpnodelay(client_socket);

	event.events = EPOLLIN; 
	event.data.fd = client_socket; 
	epoll_ctl(efd, EPOLL_CTL_ADD, client_socket, &event);


    /* I am starting now! */
	while (first_flag == 0);	
	pthread_cond_broadcast(&first);


    /* Receive all network I/O and deliver the requests to the workers */
    while (1)
    {
		// epoll 이벤트 풀에서 이벤트가 발생했는지를 검사.
		pthread_mutex_lock(&epoll_mutex);
		n = epoll_wait(efd, events, EPOLL_SIZE, -1);
		pthread_mutex_unlock(&epoll_mutex);

#ifdef __DEBUG__       
		printf("[n] : %d ",n);
#endif
		if (n == -1 ) { 
			fprintf(stderr, "epoll wait error"); 
		} 

		// 만약 이벤트가 발생했다면 발생한 이벤트의 수만큼 돌면서 데이터를 읽음.
		for (int i = 0; i < n; i++) 
		{
#ifdef __DEBUG__   
			printf("[fd] : %d\n",events[i].data.fd);
#endif
			// 만약 이벤트가 듣기 소켓에서 발생한 거라면 accept를 이용해서 연결 소켓을 생성.  
			if (events[i].data.fd == server_socket) 
			{ 
				// printf("Accept\n"); 
                clilen = sizeof(cli_addr);
                client_socket = accept(server_socket, (struct socketaddr *)&serv_addr, &clilen);
                if (client_socket < 0) {
                    printf("connection to client failed in server. \n");
                    return -1;
                }

				//printf("new client accepted\n");
				//nonblock(client_socket);
				//settcpnodelay(client_socket);

				event.events = EPOLLIN; 
				event.data.fd = client_socket; 
				epoll_ctl(efd, EPOLL_CTL_ADD, client_socket, &event);
			}
		} 

//         /* Get user request */
//         if (read(client_socket, &input_request, sizeof(Request)) > 0)                       // !!!!!!!!!!! msg 상세화?!
//         {
// #ifdef __DEBUG__
//             printf("Client Socket: %d -> Received Request: %s \n", client_socket, input_request.msg);
// #endif
//             /* Identify user request */
//             input_request.fd = client_socket;
//             Enqueue(request_queue, input_request);
//         }

        /* Thread per request Method */
        // pthread_create(&service_thr, NULL, service_dispatch, client_socket);
        // pthread_detach(service_thr);    /* The detached thread can act as daemon thread. */
        //                                 /* The parent thread doesn't need to wait. */
    }

    // pthread_mutex_destroy(&mutex);
    // pthread_cond_destroy(&not_empty);

    free(thread_handles);
    close(server_socket);

    return 0;
}

/* Peer Threads */
void * service_dispatch(void * rank)
{
    // Request popped;
    // int rv;

	char buf_in[256];
    int n, readn;
	pthread_mutex_t first_mutex;
    struct epoll_event *events;
    struct epoll_event event;
    events = (struct epoll_event *) malloc(sizeof(struct epoll_event) * EPOLL_SIZE); 

	if (first_flag == 0)
		first_flag = 1;

	pthread_mutex_lock(&first_mutex);
	pthread_cond_wait(&first, &first_mutex);
	pthread_mutex_unlock(&first_mutex);

    /* Monitor the queue to process each request */
    while (1)
    {
		pthread_mutex_lock(&epoll_mutex);
		// epoll이벤트 풀에서 이벤트가 발생했는지를 검사. 
		n = epoll_wait(efd, events, EPOLL_SIZE, -1);
		pthread_mutex_unlock(&epoll_mutex);
		if (n == -1 ) { 
			fprintf(stderr, "epoll wait error"); 
		}

#ifdef __DEBUG__     
        printf("[n] : %d ",n);
#endif

		// 만약 이벤트가 발생했다면 발생한 이벤트의 수만큼 돌면서 데이터를 읽어 들임. 
		for (int i = 0; i < n; i++)
        {
#ifdef __DEBUG__            
			printf("[fd] : %d\n",events[i].data.fd);
#endif
			if(events[i].data.fd != server_socket)
            { 
				pthread_mutex_lock(&rw_mutex);
				memset(buf_in, 0x00, 256);
				do {		
					readn = recv(client_socket, buf_in, 255,0);
				} while(readn == 0);		

				// read에 문제가 생겼다면 epoll이벤트 풀에서 제거하고 소켓을 닫음.
				if (readn < 0) 
				{
					epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd, &event);                     
					close(events[i].data.fd); 
					printf("Close fd %d \n", client_socket); 
				} 
				else
                {                  
                    printf("Client Socket: %d -> Received Request: %s \n", client_socket, buf_in);
                    do_worker_routine(buf_in, client_socket);
				}
				pthread_mutex_unlock(&rw_mutex);
			}
		}

        // popped = Dequeue(request_queue);
        // if (do_worker_routine(popped.msg, popped.fd) < 0)                   // refer to micro_httpd.h
        // {
        //     fprintf(stderr, "Error: Unable to process HTTP request \n");
        // }

        // /* Close connect */
        // close(popped.fd);
    }

    /* Close connect and terminate thread */
    // close(popped.fd);
    // pthread_exit((void *) 0);
}

int nonblock(int sock)
{
	int flags = fcntl(sock, F_GETFL);
	flags |= O_NONBLOCK;
	if(fcntl(sock,F_SETFL, flags)<0){
		perror("fcntl, executing nonblock error");
	}
	return 0;
}

int settcpnodelay(int sock)
{
	int flag = 1;
	int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
	return result;
}