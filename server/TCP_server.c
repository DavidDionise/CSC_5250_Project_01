#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>

#include "../global_utils/util.h"
#include "server_util/util.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		perror("Must include port number");
		exit(1);
	}
	
	// Initialize socket
	int socket_fd, new_socket;
	int port_number = atoi(argv[1]);

	struct sockaddr_in  server_addr, client_addr;
	int client_addr_length = sizeof(client_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_number);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error creating socket");
		exit(1);
	}

	if((bind(socket_fd, (const struct sockaddr *) &server_addr,
		(socklen_t)sizeof(server_addr))) < 0) {
		perror("Error binding");
		exit(1);
	}	

	// Initialize client data structure 
	struct clients_list c_list;
	c_list.head = 0;
	c_list.tail = 0;

	if(listen(socket_fd, 32) < 0) {
		perror("Error initializing listen");
		exit(1);
	}

	printf("Server Listening on Port %s\n", argv[1]);
	
	while(1){
		if ((new_socket = accept(socket_fd, (struct sockaddr *)&client_addr, 
			(socklen_t*)&client_addr_length)) < 0) {
			perror("Error accepting client");
			exit(1);
		}

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

		pthread_create(&tid, &attr, &handleClientCommand, (void*)&args_list);
	}

	close(socket_fd);
	close(new_socket);
	return 0;
}


