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

int main(int argc, char* argv[]) {
	if (argc < 2) {
		perror("Missing hostname and/or port number\n");
		exit(1);
	}
	char port_buffer[6];
	void* thread_arg;

	thread_arg = &port_buffer;

	int socket_fd;
	int server_port_number = atoi(argv[2]);
	struct sockaddr_in server_addr;
	
	int length, read_length;

	int registered = 0;
	int deregistering = 0;
	
	// Initialize thread
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	// Set thread as detached
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	PORT_INIT = 0;
	pthread_create(&tid, &attr, &serverRoutine, thread_arg);
	
	// Initialize socket address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port_number);
	if(inet_aton(getIP(argv[1]), &server_addr.sin_addr.s_addr) == 0) {
		perror("Error initializing server address\n");
		exit(1);
	}

	while(!deregistering) {
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);	
		connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

		if(!registered) {
			registerUser(socket_fd, &port_buffer);
			registered = 1;

			close(socket_fd);

			socket_fd = socket(AF_INET, SOCK_STREAM, 0);	
			connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		}
		
		printMenu();

		handleCommand(socket_fd, &registered ,&deregistering, &port_buffer);


		close(socket_fd);
	}

	return 0;
}


