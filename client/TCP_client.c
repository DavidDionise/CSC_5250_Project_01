#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>

#include "client_util/util.h"

#define MAX_READ_LENGTH 2048

int main(int argc, char* argv[]) {
	if (argc < 4) {
		perror("Missing hostname and/or port number\n");
		exit(1);
	}

	int socket_fd;
	int server_port_number = atoi(argv[2]);
	int my_port_number = atoi(argv[3]);
	struct sockaddr_in server_addr;
	
	char read_buffer[MAX_READ_LENGTH];
	int length, read_length;

	int deregistering;
	
	// Initialize thread
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	// Set thread as detached
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	struct args args_list;
	args_list.socket_fd = new_socket;
	args_list.client_addr = &client_addr;
	args_list.c_list = &c_list;
	args_list.f_list = &f_list;

	pthread_create(&tid, &attr, &handleClientCommand, NULL);
	
	while(!deregistering) {
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);	

		// Initialize socket address
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(server_port_number);
		if(inet_aton(getIP(argv[1]), &server_addr.sin_addr.s_addr) == 0) {
			perror("Error initializing server address\n");
			exit(1);
		}

		// Connect socket
		connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

		printMenu();

		handleCommand(socket_fd, &deregistering, my_port_number);

		close(socket_fd);
	}

	close(socket_fd);
	return 0;
}


