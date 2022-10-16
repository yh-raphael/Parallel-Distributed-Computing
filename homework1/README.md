Parallel Distributed Computing
Author: Raphael
Date  : October 16, 2022

# Multi-threaded Server and Client Program

## How to set .html file location?

"/var/tmp/raphael/" is the path of html files.
I made a set of files like below.

/var/tmp/raphael/file_1.html
/var/tmp/raphael/file_2.html
/var/tmp/raphael/file_3.html
/var/tmp/raphael/file_4.html
/var/tmp/raphael/file_5.html


## What is "list.txt" file?

In this file, you can find the list of .html files.
In addition, you can find the number of files on the first line of the file!


## How to compile the program?

I made two Makefiles in each directory: master_worker and peer.
Please enter 'make' into the command line on each path.


## How to run Master-Worker Model?

In 'master_worker' directory, you can run both server and client programs which are implemented using single queue.

### Server
usage: ./server <port_id> <# of workers>

e.g. : ./server 4000 3

### Client
usage: ./client <server_ip> <server_port_id> <# of workers> <# of access> <list file path>

e.g. : ./client 163.239.27.139 4000 2 2 list.txt


## How to run Peer Model?

In 'peer' directory, you can run both server and client programs which are implemented using epoll mechanism.

### Server
usage: ./epoll_server <port_id> <# of workers>

e.g. : ./epoll_server 4000 3

### Client
usage: ./epoll_client <server_ip> <server_port_id> <# of workers> <# of access> <list file path>

e.g. : ./epoll_client 163.239.27.139 4000 2 2 list.txt


Thnanks for your reading!