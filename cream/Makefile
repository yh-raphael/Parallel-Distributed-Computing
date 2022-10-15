all: server

server:
	gcc -o server pth_server.c queue.c my_micro_httpd.h -lpthread

clean :
	rm -rf server