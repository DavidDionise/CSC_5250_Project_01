
#include <stdio.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <strings.h>

#include "server_util/util.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		perror("Must include port number");
		exit(1);
	}
	
	int socket_fd, new_socket;
	int port_number = atoi(argv[1]);
	pid_t pid;

	struct sockaddr_in  server_addr, client_addr;

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

	int client_addr_length = sizeof(client_addr);

	puts("Server Starting");

	if(listen(socket_fd, 32) < 0) {
		perror("Error initializing listen");
		exit(1);
	}
	
	while(1){
		if ((new_socket = accept(socket_fd, (struct sockaddr *)&client_addr, 
			(socklen_t*)&client_addr_length)) < 0) {
			perror("Error accepting client");
			exit(1);
		}

		pid = fork();

		// Child process
		if(pid == 0) {

			// Initialize client data structure 
			struct clients_list c_list;
			c_list.head = 0;
			c_list.tail = 0;

			handleClientCommand(new_socket, client_addr, &c_list);
			exit(0);
		}

		// Error forking
		else if(pid < 0)
			perror("Error forking new process.");
	}

	close(socket_fd);
	close(new_socket);
	return 0;
}


