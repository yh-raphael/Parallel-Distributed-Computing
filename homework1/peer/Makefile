all: epoll_server epoll_client

server:
	gcc -o epoll_server epoll_server.c my_micro_httpd.h timer.h -lpthread

client:
	gcc -o epoll_client epoll_client.c -lpthread

clean :
	rm -rf epoll_server epoll_client