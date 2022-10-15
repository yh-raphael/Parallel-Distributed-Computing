all: server client

server:
	gcc -o server pth_server.c queue.c my_micro_httpd.h -lpthread

client:
	gcc -o client pth_client.c -lpthread

clean :
	rm -rf server client